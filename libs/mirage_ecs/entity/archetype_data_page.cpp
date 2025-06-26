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

ArchetypeDataPage::ArchetypeDataPage(Buffer&& buffer)
    : buffer_(std::move(buffer)) {}

ArchetypeDataPage::~ArchetypeDataPage() { Reset(); }

void ArchetypeDataPage::Initialize(SharedDescriptor&& descriptor) {
  MIRAGE_DCHECK(descriptor->align() <= buffer_.align());
  MIRAGE_DCHECK(descriptor->size() <= buffer_.size());
  Reset();
  descriptor_ = std::move(descriptor);
  const auto capacity = buffer_.size() / descriptor_->size();
  entity_id_array_.set_size(capacity);
}

void ArchetypeDataPage::Reset() {
  Clear();
  entity_id_array_.Clear();
  descriptor_ = nullptr;
}

void ArchetypeDataPage::Clear() {
  if (!is_initialized()) {
    return;
  }

  const auto capacity = entity_id_array_.size();
  for (auto i = 0; i < capacity; ++i) {
    if (!entity_id_array_[i].is_valid()) continue;

    std::byte* target = buffer_.ptr() + i * descriptor_->size();
    for (const auto& entry : descriptor_->offset_map()) {
      const auto component_id = entry.key();
      const auto offset = entry.val();
      component_id.destruct_func()(target + offset);
    }
  }
}

void ArchetypeDataPage::Reserve(const size_t new_buffer_size) {
  if (new_buffer_size <= buffer_.size()) {
    return;
  }
  auto new_buffer = Buffer(new_buffer_size, buffer_.align());
  if (!is_initialized()) {
    buffer_ = std::move(new_buffer);
    return;
  }

  const auto capacity = entity_id_array_.size();
  for (const auto& entry : descriptor_->offset_map()) {
    const auto component_id = entry.key();
    const auto offset = entry.val();

    for (auto i = 0; i < capacity; ++i) {
      if (!entity_id_array_[i].is_valid()) continue;

      const auto component_offset = i * descriptor_->size() + offset;
      std::byte* target = buffer_.ptr() + component_offset;
      std::byte* dest = new_buffer.ptr() + component_offset;
      component_id.move_func()(target, dest);
    }
  }
  buffer_ = std::move(new_buffer);

  const auto new_capacity = new_buffer_size / descriptor_->size();
  entity_id_array_.set_size(new_capacity);
}

ConstView ArchetypeDataPage::operator[](const int32_t index) const {
  MIRAGE_DCHECK(is_initialized());
  return {descriptor_.raw_ptr(), buffer_.ptr() + index * descriptor_->size(),
          entity_id_array_.begin() + index};
}

View ArchetypeDataPage::operator[](const int32_t index) {
  MIRAGE_DCHECK(is_initialized());
  return {descriptor_.raw_ptr(), buffer_.ptr() + index * descriptor_->size(),
          entity_id_array_.begin() + index};
}

ConstIterator ArchetypeDataPage::begin() const {
  return ConstIterator((*this)[0]);
}

ConstIterator ArchetypeDataPage::end() const {
  return begin() +
         static_cast<ConstIterator::difference_type>(entity_id_array_.size());
}

Iterator ArchetypeDataPage::begin() { return Iterator((*this)[0]); }

Iterator ArchetypeDataPage::end() {
  return begin() +
         static_cast<ConstIterator::difference_type>(entity_id_array_.size());
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

bool ConstView::is_null() const { return descriptor_ == nullptr; }

ConstView::ConstView(const ArchetypeDescriptor* descriptor,
                     const std::byte* view_ptr,
                     const Array<EntityId>::ConstIterator entity_id_iter)
    : descriptor_(descriptor),
      view_ptr_(view_ptr),
      entity_id_iter_(entity_id_iter) {}

ConstIterator::ConstIterator(const ConstView& view) : view_(view) {}

ConstIterator::ConstIterator(std::nullptr_t) {}

ConstIterator& ConstIterator::operator=(std::nullptr_t) {
  view_.descriptor_ = nullptr;
  view_.view_ptr_ = nullptr;
  view_.entity_id_iter_ = nullptr;
  return *this;
}

ConstIterator::reference ConstIterator::operator*() const { return view_; }

ConstIterator::pointer ConstIterator::operator->() const { return &view_; }

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

EntityId& View::entity_id() { return *entity_id_iter_; }

bool View::is_null() const { return descriptor_ == nullptr; }

View::View(const ArchetypeDescriptor* descriptor, std::byte* view_ptr,
           const Array<EntityId>::Iterator entity_id_iter)
    : descriptor_(descriptor),
      view_ptr_(view_ptr),
      entity_id_iter_(entity_id_iter) {}

Iterator::Iterator(const View& view) : view_(view) {}

Iterator::Iterator(std::nullptr_t) {}

Iterator& Iterator::operator=(std::nullptr_t) {
  view_.descriptor_ = nullptr;
  view_.view_ptr_ = nullptr;
  view_.entity_id_iter_ = nullptr;
  return *this;
}

Iterator::reference Iterator::operator*() const { return view_; }

Iterator::pointer Iterator::operator->() const { return &view_; }

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
