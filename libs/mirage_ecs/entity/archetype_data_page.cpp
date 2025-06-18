#include "mirage_ecs/entity/archetype_data_page.hpp"

#include <algorithm>
#include <cstddef>
#include <utility>

#include "mirage_base/define/check.hpp"
#include "mirage_base/wrap/box.hpp"
#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/component/component_id.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"
#include "mirage_ecs/entity/entity_id.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

using SharedDescriptor = ArchetypeDataPage::SharedDescriptor;
using Buffer = ArchetypeDataPage::Buffer;
using ConstView = ArchetypeDataPage::ConstView;
using ConstIterator = ArchetypeDataPage::ConstIterator;
using View = ArchetypeDataPage::View;
using Iterator = ArchetypeDataPage::Iterator;
using Courier = ArchetypeDataPage::Courier;

ArchetypeDataPage::ArchetypeDataPage(const size_t buffer_size,
                                     const size_t align)
    : buffer_(buffer_size, align) {}

ArchetypeDataPage::~ArchetypeDataPage() { Reset(); }

void ArchetypeDataPage::Initialize(SharedDescriptor&& descriptor) {
  MIRAGE_DCHECK(descriptor->align() <= buffer_.align());
  MIRAGE_DCHECK(descriptor->size() <= buffer_.size());
  Reset();
  descriptor_ = std::move(descriptor);
  const auto capacity = buffer_.size() / descriptor_->size();
  entity_id_array_.Reserve(capacity);

  dense_.Reserve(capacity);
  sparse_.Reserve(capacity);
  hole_.Reserve(capacity);
  for (auto i = 0; i < capacity; ++i) {
    // Initialize sparse array with -1
    sparse_.Push(-1);
    // Fill hole with indices in reverse order
    hole_.Push(static_cast<int32_t>(entity_id_array_.capacity()) - i - 1);
  }
}

void ArchetypeDataPage::Reset() {
  Clear();
  entity_id_array_.Clear();
  descriptor_ = nullptr;
  dense_.Clear();
  sparse_.Clear();
  hole_.Clear();
}

void ArchetypeDataPage::Reserve(size_t new_buffer_size) {
  if (new_buffer_size <= buffer_.size()) {
    return;
  }
  auto new_buffer = Buffer(new_buffer_size, buffer_.align());
  if (!is_initialized()) {
    buffer_ = std::move(new_buffer);
    return;
  }

  const auto s = size();
  for (const auto& entry : descriptor_->offset_map()) {
    const auto component_id = entry.key();
    const auto offset = entry.val();

    for (auto i = 0; i < s; ++i) {
      const auto component_offset = i * descriptor_->size() + offset;
      std::byte* target = buffer_.ptr() + component_offset;
      std::byte* dest = new_buffer.ptr() + component_offset;
      component_id.move_func()(target, dest);
    }
  }
  buffer_ = std::move(new_buffer);

  const auto new_capacity = new_buffer_size / descriptor_->size();
  const auto fill_cnt = new_capacity - capacity();
  entity_id_array_.Reserve(new_capacity);
  dense_.Reserve(new_capacity);
  sparse_.Reserve(new_capacity);
  hole_.Reserve(new_capacity);
  for (auto i = 0; i < fill_cnt; ++i) {
    // Initialize sparse array with -1
    sparse_.Push(-1);
    // Fill hole with indices in reverse order
    hole_.Push(static_cast<int32_t>(new_capacity) - i - 1);
  }
}

int32_t ArchetypeDataPage::Push(const EntityId& id, ComponentBundle& bundle) {
  MIRAGE_DCHECK(is_initialized());
  if (hole_.empty()) {
    return -1;
  }
  std::byte* dest = buffer_.ptr() + size() * descriptor_->size();
  entity_id_array_.Push(id);
  for (const auto& entry : descriptor_->offset_map()) {
    const auto component_id = entry.key();
    const auto offset = entry.val();

    Box<Component> component = bundle.Remove(component_id.type_id()).Unwrap();
    component_id.move_func()(component.raw_ptr(), dest + offset);
  }

  const auto sparse_id = hole_.Pop();
  sparse_[sparse_id] = static_cast<int32_t>(dense_.size());
  dense_.Push(sparse_id);

  return sparse_id;
}

int32_t ArchetypeDataPage::Push(View& view) {
  MIRAGE_DCHECK(is_initialized());
  if (hole_.empty()) {
    return -1;
  }

  std::byte* dest = buffer_.ptr() + size() * descriptor_->size();
  entity_id_array_.Push(view.entity_id());
  for (const auto& entry : descriptor_->offset_map()) {
    const auto component_id = entry.key();
    const auto offset = entry.val();

    void* component = view.TryGet(component_id);
    MIRAGE_DCHECK(component != nullptr);
    component_id.move_func()(component, dest + offset);
  }

  const auto sparse_id = hole_.Pop();
  sparse_[sparse_id] = static_cast<int32_t>(dense_.size());
  dense_.Push(sparse_id);
  return sparse_id;
}

Courier ArchetypeDataPage::Take(const int32_t sparse_id) {
  return TakeMany({sparse_id});
}

Courier ArchetypeDataPage::TakeMany(const Array<int32_t>& sparse_id_array) {
  return TakeMany(sparse_id_array, descriptor_);
}

Courier ArchetypeDataPage::Take(const int32_t sparse_id,
                                const SharedDescriptor& descriptor) {
  return TakeMany({sparse_id}, descriptor);
}

Courier ArchetypeDataPage::TakeMany(const Array<int32_t>& sparse_id_array,
                                    const SharedDescriptor& descriptor) {
  MIRAGE_DCHECK(is_initialized());
  Array<int32_t> dense_id_array = MapSparseToDense(sparse_id_array);
  auto rv = Courier(descriptor, *this, dense_id_array);
  SwapRemoveManyDense(std::move(dense_id_array));
  return rv;
}

void ArchetypeDataPage::Remove(const int32_t sparse_id) {
  MIRAGE_DCHECK(is_initialized());
  SwapRemoveDense(sparse_[sparse_id]);
}

void ArchetypeDataPage::RemoveMany(const Array<int32_t>& sparse_id_array) {
  MIRAGE_DCHECK(is_initialized());
  SwapRemoveManyDense(MapSparseToDense(sparse_id_array));
}

void ArchetypeDataPage::Clear() {
  if (!is_initialized()) {
    return;
  }
  for (auto i = 0; i < size(); ++i) {
    std::byte* target = buffer_.ptr() + i * descriptor_->size();
    for (const auto& entry : descriptor_->offset_map()) {
      const auto component_id = entry.key();
      const auto offset = entry.val();
      component_id.destruct_func()(target + offset);
    }
  }
  descriptor_.Reset();
  entity_id_array_.Clear();
  dense_.Clear();
  sparse_.Clear();
  hole_.Clear();
}

ConstView ArchetypeDataPage::operator[](const int32_t sparse_id) const {
  MIRAGE_DCHECK(is_initialized());
  const auto dense_id = sparse_[sparse_id];
  MIRAGE_DCHECK(dense_id >= 0 && dense_id < size());
  return {descriptor_.raw_ptr(), buffer_.ptr() + dense_id * descriptor_->size(),
          entity_id_array_.begin() + dense_id};
}

View ArchetypeDataPage::operator[](const int32_t sparse_id) {
  MIRAGE_DCHECK(is_initialized());
  const auto dense_id = sparse_[sparse_id];
  MIRAGE_DCHECK(dense_id >= 0 && dense_id < size());
  return {descriptor_.raw_ptr(), buffer_.ptr() + dense_id * descriptor_->size(),
          entity_id_array_.begin() + dense_id};
}

ConstIterator ArchetypeDataPage::begin() const { return ConstIterator(*this); }

ConstIterator ArchetypeDataPage::end() const {
  return begin() + static_cast<ConstIterator::difference_type>(size());
}

Iterator ArchetypeDataPage::begin() { return Iterator(*this); }

Iterator ArchetypeDataPage::end() {
  return begin() + static_cast<ConstIterator::difference_type>(size());
}

bool ArchetypeDataPage::is_initialized() const {
  return descriptor_ != nullptr;
}

const SharedDescriptor& ArchetypeDataPage::descriptor() const {
  return descriptor_;
}

const Array<EntityId>& ArchetypeDataPage::entity_id_array() const {
  return entity_id_array_;
}

size_t ArchetypeDataPage::capacity() const {
  return entity_id_array_.capacity();
}

size_t ArchetypeDataPage::size() const { return entity_id_array_.size(); }

const Buffer& ArchetypeDataPage::buffer() const { return buffer_; }

Buffer& ArchetypeDataPage::buffer() { return buffer_; }

const Array<int32_t>& ArchetypeDataPage::dense() const { return dense_; }

const Array<int32_t>& ArchetypeDataPage::sparse() const { return sparse_; }

const Array<int32_t>& ArchetypeDataPage::hole() const { return hole_; }

Array<int32_t> ArchetypeDataPage::MapSparseToDense(
    const Array<int32_t>& sparse_id_array) const {
  Array<int32_t> dense_id_array;
  dense_id_array.Reserve(sparse_id_array.size());
  for (const auto sparse_id : sparse_id_array) {
    dense_id_array.Push(sparse_[sparse_id]);
  }
  return dense_id_array;
}

void ArchetypeDataPage::SwapRemoveDense(const int32_t dense_id) {
  MIRAGE_DCHECK(dense_id >= 0);
  entity_id_array_.SwapRemove(dense_id);
  std::byte* last = buffer_.ptr() + size() * descriptor_->size();
  std::byte* dest = buffer_.ptr() + dense_id * descriptor_->size();
  for (const auto& entry : descriptor_->offset_map()) {
    const auto component_id = entry.key();
    const auto offset = entry.val();

    component_id.destruct_func()(dest + offset);
    component_id.move_func()(last + offset, dest + offset);
    component_id.destruct_func()(last + offset);
  }

  sparse_[dense_.Tail()] = dense_id;
  const auto hole_id = dense_.SwapTake(dense_id);
  hole_.Push(hole_id);
  sparse_[hole_id] = -1;
}

void ArchetypeDataPage::SwapRemoveManyDense(Array<int32_t>&& dense_id_array) {
  std::ranges::sort(dense_id_array, std::greater());

  for (const auto index : dense_id_array) {
    SwapRemoveDense(index);
  }
}

const void* ConstView::TryGet(const ComponentId id) const {
  const auto& offset_map = descriptor_->offset_map();
  const auto iter = offset_map.TryFind(id);
  if (iter == offset_map.end()) {
    return nullptr;
  }
  return view_ptr_ + iter->val();
}

const EntityId& ConstView::entity_id() const { return *entity_id_iter_; }

bool ConstView::is_null() const { return descriptor_ == nullptr; }

ConstView::ConstView(const ArchetypeDescriptor* descriptor,
                     const std::byte* view_ptr,
                     const Array<EntityId>::ConstIterator entity_id_iter)
    : descriptor_(descriptor),
      view_ptr_(view_ptr),
      entity_id_iter_(entity_id_iter) {}

ConstIterator::ConstIterator(const ArchetypeDataPage& page)
    : view_(page.descriptor().raw_ptr(), page.buffer().ptr(),
            page.entity_id_array_.begin()) {}

ConstIterator::ConstIterator(const Courier& courier)
    : view_(courier.descriptor().raw_ptr(), courier.buffer().ptr(),
            courier.entity_id_array().begin()) {}

ConstIterator::ConstIterator(std::nullptr_t) {}

ConstIterator& ConstIterator::operator=(std::nullptr_t) {
  view_.descriptor_ = nullptr;
  view_.view_ptr_ = nullptr;
  view_.entity_id_iter_ = nullptr;
  return *this;
}

ConstIterator::reference ConstIterator::operator*() const { return view_; }

ConstIterator::pointer ConstIterator::operator->() const { return view_; }

ConstIterator::reference ConstIterator::operator[](
    const difference_type diff) const {
  ConstView rv = view_;
  rv.view_ptr_ += diff * rv.descriptor_->size();
  rv.entity_id_iter_ += diff;
  return rv;
}

ConstIterator::iterator_type& ConstIterator::operator++() {
  view_.view_ptr_ += view_.descriptor_->size();
  ++view_.entity_id_iter_;
  return *this;
}

ConstIterator::iterator_type ConstIterator::operator++(int) {
  const iterator_type rv = *this;
  ++(*this);
  return rv;
}

ConstIterator::iterator_type& ConstIterator::operator--() {
  view_.view_ptr_ -= view_.descriptor_->size();
  --view_.entity_id_iter_;
  return *this;
}

ConstIterator::iterator_type ConstIterator::operator--(int) {
  const iterator_type rv = *this;
  --(*this);
  return rv;
}

ConstIterator::iterator_type& ConstIterator::operator+=(
    const difference_type diff) {
  view_.view_ptr_ += diff * view_.descriptor_->size();
  view_.entity_id_iter_ += diff;
  return *this;
}

ConstIterator::iterator_type ConstIterator::operator+(
    const difference_type diff) const {
  iterator_type rv = *this;
  rv += diff;
  return rv;
}

ConstIterator::iterator_type operator+(const ptrdiff_t diff,
                                       const ConstIterator& iter) {
  return iter + diff;
}

ConstIterator::iterator_type& ConstIterator::operator-=(
    const difference_type diff) {
  view_.view_ptr_ -= diff * view_.descriptor_->size();
  view_.entity_id_iter_ -= diff;
  return *this;
}

ConstIterator::iterator_type ConstIterator::operator-(
    const difference_type diff) const {
  iterator_type rv = *this;
  rv -= diff;
  return rv;
}

ConstIterator::difference_type ConstIterator::operator-(
    const iterator_type& other) const {
  MIRAGE_DCHECK(view_.descriptor_ == other.view_.descriptor_);
  return view_.entity_id_iter_ - other.view_.entity_id_iter_;
}

std::strong_ordering ConstIterator::operator<=>(
    const iterator_type& other) const {
  MIRAGE_DCHECK(view_.descriptor_ == other.view_.descriptor_);
  return view_.entity_id_iter_ <=> other.view_.entity_id_iter_;
}

bool ConstIterator::operator==(const iterator_type& other) const {
  return view_.entity_id_iter_ == other.view_.entity_id_iter_;
}

ConstIterator::operator bool() const { return !is_null(); }

bool ConstIterator::is_null() const { return view_.view_ptr_ == nullptr; }

const void* View::TryGet(const ComponentId id) const {
  const auto& offset_map = descriptor_->offset_map();
  const auto iter = offset_map.TryFind(id);
  if (iter == offset_map.end()) {
    return nullptr;
  }
  return view_ptr_ + iter->val();
}

void* View::TryGet(const ComponentId id) {
  const auto& offset_map = descriptor_->offset_map();
  const auto iter = offset_map.TryFind(id);
  if (iter == offset_map.end()) {
    return nullptr;
  }
  return view_ptr_ + iter->val();
}

const EntityId& View::entity_id() const { return *entity_id_iter_; }

bool View::is_null() const { return descriptor_ == nullptr; }

View::View(const ArchetypeDescriptor* descriptor, std::byte* view_ptr,
           const Array<EntityId>::ConstIterator entity_id_iter)
    : descriptor_(descriptor),
      view_ptr_(view_ptr),
      entity_id_iter_(entity_id_iter) {}

Iterator::Iterator(ArchetypeDataPage& page)
    : view_(page.descriptor().raw_ptr(), page.buffer().ptr(),
            page.entity_id_array().begin()) {}

Iterator::Iterator(Courier& courier)
    : view_(courier.descriptor().raw_ptr(), courier.buffer().ptr(),
            courier.entity_id_array().begin()) {}

Iterator::Iterator(std::nullptr_t) {}

Iterator& Iterator::operator=(std::nullptr_t) {
  view_.descriptor_ = nullptr;
  view_.view_ptr_ = nullptr;
  view_.entity_id_iter_ = nullptr;
  return *this;
}

Iterator::reference Iterator::operator*() const { return view_; }

Iterator::pointer Iterator::operator->() const { return view_; }

Iterator::reference Iterator::operator[](const difference_type diff) const {
  View rv = view_;
  rv.view_ptr_ += diff * rv.descriptor_->size();
  rv.entity_id_iter_ += diff;
  return rv;
}

Iterator::iterator_type& Iterator::operator++() {
  view_.view_ptr_ += view_.descriptor_->size();
  ++view_.entity_id_iter_;
  return *this;
}

Iterator::iterator_type Iterator::operator++(int) {
  const iterator_type rv = *this;
  ++(*this);
  return rv;
}

Iterator::iterator_type& Iterator::operator--() {
  view_.view_ptr_ -= view_.descriptor_->size();
  --view_.entity_id_iter_;
  return *this;
}

Iterator::iterator_type Iterator::operator--(int) {
  const iterator_type rv = *this;
  --(*this);
  return rv;
}

Iterator::iterator_type& Iterator::operator+=(const difference_type diff) {
  view_.view_ptr_ += diff * view_.descriptor_->size();
  view_.entity_id_iter_ += diff;
  return *this;
}

Iterator::iterator_type Iterator::operator+(const difference_type diff) const {
  iterator_type rv = *this;
  rv += diff;
  return rv;
}

Iterator::iterator_type operator+(const ptrdiff_t diff, const Iterator& iter) {
  return iter + diff;
}

Iterator::iterator_type& Iterator::operator-=(const difference_type diff) {
  view_.view_ptr_ -= diff * view_.descriptor_->size();
  view_.entity_id_iter_ -= diff;
  return *this;
}

Iterator::iterator_type Iterator::operator-(const difference_type diff) const {
  iterator_type rv = *this;
  rv -= diff;
  return rv;
}

Iterator::difference_type Iterator::operator-(
    const iterator_type& other) const {
  MIRAGE_DCHECK(view_.descriptor_ == other.view_.descriptor_);
  return view_.view_ptr_ - other.view_.view_ptr_;
}

std::strong_ordering Iterator::operator<=>(const iterator_type& other) const {
  MIRAGE_DCHECK(view_.descriptor_ == other.view_.descriptor_);
  return view_.entity_id_iter_ <=> other.view_.entity_id_iter_;
}

bool Iterator::operator==(const iterator_type& other) const {
  return view_.entity_id_iter_ == other.view_.entity_id_iter_;
}

Iterator::operator bool() const { return !is_null(); }

bool Iterator::is_null() const { return view_.view_ptr_ == nullptr; }

Courier::~Courier() {
  if (is_null()) {
    return;
  }
  const size_t size = this->size();
  std::byte* dest = buffer_.ptr();
  for (size_t i = 0; i < size; ++i) {
    for (const auto& entry : descriptor_->offset_map()) {
      const auto component_id = entry.key();
      const auto offset = entry.val();
      component_id.destruct_func()(dest + offset);
    }
    dest += i * descriptor_->size();
  }
}

Courier::operator bool() const { return !is_null(); }

bool Courier::is_null() const { return descriptor_ == nullptr; }

ConstIterator Courier::begin() const { return ConstIterator(*this); }

ConstIterator Courier::end() const { return begin() + ssize(); }

Iterator Courier::begin() { return Iterator(*this); }

Iterator Courier::end() { return begin() + ssize(); }

const SharedDescriptor& Courier::descriptor() const { return descriptor_; }

const Array<EntityId>& Courier::entity_id_array() const {
  return entity_id_array_;
}

size_t Courier::size() const { return buffer_.size() / descriptor_->size(); }

ptrdiff_t Courier::ssize() const {
  const auto ssize = static_cast<ptrdiff_t>(size());
  MIRAGE_DCHECK(ssize > 0);
  return ssize;
}

const Buffer& Courier::buffer() const { return buffer_; }

Buffer& Courier::buffer() { return buffer_; }

Courier::Courier(const SharedDescriptor& descriptor, ArchetypeDataPage& page,
                 const Array<int32_t>& dense_id_array)
    : descriptor_(descriptor.Clone()),
      buffer_(Buffer(descriptor_->size() * dense_id_array.size(),
                     descriptor_->align())) {
  const auto& page_descriptor = page.descriptor();
  const auto& page_offset_map = page_descriptor->offset_map();

  std::byte* dest = buffer_.ptr();
  for (const size_t index : dense_id_array) {
    MIRAGE_DCHECK(index < page.size());
    entity_id_array_.Push(page.entity_id_array_[index]);
    page.entity_id_array_[index].Reset();

    std::byte* target = page.buffer().ptr() + index * page_descriptor->size();
    for (const auto& entry : descriptor_->offset_map()) {
      const auto component_id = entry.key();
      const auto offset = entry.val();

      const auto page_offset_iter = page_offset_map.TryFind(component_id);
      if (page_offset_iter == page_offset_map.end()) {
        continue;
      }
      component_id.move_func()(target + page_offset_iter->val(), dest + offset);
    }
    dest += descriptor_->size();
  }
}
