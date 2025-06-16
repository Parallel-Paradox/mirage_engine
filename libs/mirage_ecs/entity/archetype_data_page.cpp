#include "mirage_ecs/entity/archetype_data_page.hpp"

#include <algorithm>
#include <cstddef>

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

ArchetypeDataPage::ArchetypeDataPage(ArchetypeDataPage&& other) noexcept
    : descriptor_(std::move(other.descriptor_)),
      entity_id_array_(std::move(other.entity_id_array_)),
      buffer_(std::move(other.buffer_)) {
  other.descriptor_ = nullptr;
}

ArchetypeDataPage& ArchetypeDataPage::operator=(
    ArchetypeDataPage&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~ArchetypeDataPage();
  new (this) ArchetypeDataPage(std::move(other));
  return *this;
}

void ArchetypeDataPage::Initialize(SharedDescriptor descriptor) {
  MIRAGE_DCHECK(descriptor->align() <= buffer_.align());
  Reset();
  entity_id_array_.Reserve(buffer_.size() / descriptor->size());
  descriptor_ = std::move(descriptor);
}

void ArchetypeDataPage::Reset() {
  Clear();
  entity_id_array_.Clear();
  descriptor_ = nullptr;
}

bool ArchetypeDataPage::Push(EntityId id, ComponentBundle& bundle) {
  MIRAGE_DCHECK(is_initialized());
  if (size() >= capacity()) {
    return false;
  }
  std::byte* dest = buffer_.ptr() + size() * descriptor_->size();
  entity_id_array_.Push(id);
  for (const auto& entry : descriptor_->offset_map()) {
    const auto component_id = entry.key();
    const auto offset = entry.val();

    Box<Component> component = bundle.Remove(component_id.type_id()).Unwrap();
    component_id.move_func()(component.raw_ptr(), dest + offset);
  }
  return true;
}

bool ArchetypeDataPage::Push(View& view) {
  MIRAGE_DCHECK(is_initialized());
  if (size() >= capacity()) {
    return false;
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
  return true;
}

Courier ArchetypeDataPage::SwapPop(const size_t index) {
  return SwapPopMany({index});
}

Courier ArchetypeDataPage::SwapPopMany(Array<size_t> index_array) {
  MIRAGE_DCHECK(is_initialized());
  MIRAGE_DCHECK(index_array.size() > 0);
  auto rv = Courier(*this, index_array);
  SwapRemoveMany(std::move(index_array));
  return rv;
}

void ArchetypeDataPage::SwapRemove(const size_t index) {
  return SwapRemoveMany({index});
}

void ArchetypeDataPage::SwapRemoveMany(Array<size_t> index_array) {
  MIRAGE_DCHECK(is_initialized());
  MIRAGE_DCHECK(index_array.size() > 0);

  std::ranges::sort(index_array, std::greater());

  for (const size_t index : index_array) {
    MIRAGE_DCHECK(index < size());
    entity_id_array_.SwapRemove(index);
    std::byte* last = buffer_.ptr() + size() * descriptor_->size();
    std::byte* dest = buffer_.ptr() + index * descriptor_->size();
    for (const auto& entry : descriptor_->offset_map()) {
      const auto component_id = entry.key();
      const auto offset = entry.val();

      component_id.destruct_func()(dest + offset);
      component_id.move_func()(last + offset, dest + offset);
      component_id.destruct_func()(last + offset);
    }
  }
}

void ArchetypeDataPage::Clear() {
  if (!is_initialized()) {
    return;
  }
  while (size() > 0) {
    entity_id_array_.Pop();
    std::byte* target = buffer_.ptr() + size() * descriptor_->size();
    for (const auto& entry : descriptor_->offset_map()) {
      const auto component_id = entry.key();
      const auto offset = entry.val();
      component_id.destruct_func()(target + offset);
    }
  }
}

ConstView ArchetypeDataPage::operator[](const size_t index) const {
  MIRAGE_DCHECK(is_initialized());
  MIRAGE_DCHECK(index < size());
  return {descriptor_.raw_ptr(), buffer_.ptr() + index * descriptor_->size(),
          entity_id_array_.begin() + index};
}

View ArchetypeDataPage::operator[](const size_t index) {
  MIRAGE_DCHECK(is_initialized());
  MIRAGE_DCHECK(index < size());
  return {descriptor_.raw_ptr(), buffer_.ptr() + index * descriptor_->size(),
          entity_id_array_.begin() + index};
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

const void* ConstView::TryGet(const ComponentId id) const {
  const auto& offset_map = descriptor_->offset_map();
  const auto iter = offset_map.TryFind(id);
  if (iter == offset_map.end()) {
    return nullptr;
  }
  return view_ptr_ + iter->val();
}

const EntityId& ConstView::entity_id() const { return *entity_id_iter_; }

ConstView::ConstView(const ArchetypeDescriptor* descriptor,
                     const std::byte* view_ptr,
                     Array<EntityId>::ConstIterator entity_id_iter)
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
  view_.entity_id_iter_++;
  return *this;
}

ConstIterator::iterator_type ConstIterator::operator++(int) {
  const iterator_type rv = *this;
  ++(*this);
  return rv;
}

ConstIterator::iterator_type& ConstIterator::operator--() {
  view_.view_ptr_ -= view_.descriptor_->size();
  view_.entity_id_iter_--;
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

View::View(const ArchetypeDescriptor* descriptor, std::byte* view_ptr,
           Array<EntityId>::ConstIterator entity_id_iter)
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
  view_.entity_id_iter_++;
  return *this;
}

Iterator::iterator_type Iterator::operator++(int) {
  const iterator_type rv = *this;
  ++(*this);
  return rv;
}

Iterator::iterator_type& Iterator::operator--() {
  view_.view_ptr_ -= view_.descriptor_->size();
  view_.entity_id_iter_--;
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

Courier::Courier(ArchetypeDataPage& page, const Array<size_t>& index_array)
    : descriptor_(page.descriptor_.Clone()),
      buffer_(Buffer(descriptor_->size() * index_array.size(),
                     descriptor_->align())) {
  std::byte* dest = buffer_.ptr();
  for (const size_t index : index_array) {
    MIRAGE_DCHECK(index < page.size());
    entity_id_array_.Emplace(std::move(page.entity_id_array_[index]));

    std::byte* target = page.buffer().ptr() + index * descriptor_->size();
    for (const auto& entry : descriptor_->offset_map()) {
      const auto component_id = entry.key();
      const auto offset = entry.val();
      component_id.move_func()(target + offset, dest + offset);
    }
    dest += descriptor_->size();
  }
}
