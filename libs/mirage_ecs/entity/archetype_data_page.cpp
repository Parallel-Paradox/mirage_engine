#include "mirage_ecs/entity/archetype_data_page.hpp"

#include <cstddef>
#include <iterator>

#include "mirage_base/define/check.hpp"
#include "mirage_base/wrap/box.hpp"
#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/component/component_id.hpp"

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
      capacity_(other.capacity_),
      size_(other.size_),
      buffer_(std::move(other.buffer_)) {
  other.descriptor_ = nullptr;
  other.capacity_ = 0;
  other.size_ = 0;
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
  descriptor_ = std::move(descriptor);
  capacity_ = buffer_.size() / descriptor_->size();
}

void ArchetypeDataPage::Reset() {
  Clear();
  descriptor_ = nullptr;
  capacity_ = 0;
  size_ = 0;
}

bool ArchetypeDataPage::Push(ComponentBundle& bundle) {
  MIRAGE_DCHECK(is_initialized());
  if (size_ >= capacity_) {
    return false;
  }
  std::byte* dest = buffer_.ptr() + size_ * descriptor_->size();
  for (const auto& entry : descriptor_->offset_map()) {
    const auto component_id = entry.key();
    const auto offset = entry.val();

    Box<Component> component = bundle.Remove(component_id.type_id()).Unwrap();
    component_id.move_func()(component.raw_ptr(), dest + offset);
  }
  size_ += 1;
  return true;
}

bool ArchetypeDataPage::Push(Courier&& slice) {
  MIRAGE_DCHECK(is_initialized());
  if (size_ >= capacity_) {
    return false;
  }
  for (auto view : slice) {
    std::byte* dest = buffer_.ptr() + size_ * descriptor_->size();
    size_ += 1;
    for (const auto& entry : descriptor_->offset_map()) {
      const auto component_id = entry.key();
      const auto offset = entry.val();

      void* component = view.TryGet(component_id);
      MIRAGE_DCHECK(component != nullptr);
      component_id.move_func()(component, dest + offset);
    }
  }
  return true;
}

Courier ArchetypeDataPage::SwapPop(const size_t index) {
  return SwapPopMany({index});
}

Courier ArchetypeDataPage::SwapPopMany(
    const std::initializer_list<size_t> index_list) {
  MIRAGE_DCHECK(is_initialized());
  MIRAGE_DCHECK(index_list.size() > 0);
  auto rv = Courier(*this, index_list);
  SwapRemoveMany(index_list);
  return rv;
}

void ArchetypeDataPage::SwapRemove(const size_t index) {
  return SwapRemoveMany({index});
}

void ArchetypeDataPage::SwapRemoveMany(
    const std::initializer_list<size_t> index_list) {
  MIRAGE_DCHECK(is_initialized());
  MIRAGE_DCHECK(index_list.size() > 0);
  for (const size_t index : index_list) {
    MIRAGE_DCHECK(index < size_);
    size_ -= 1;
    std::byte* last = buffer_.ptr() + size_ * descriptor_->size();
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
  for (size_t i = 0; i < size_; ++i) {
    std::byte* target = buffer_.ptr() + i * descriptor_->size();
    for (const auto& entry : descriptor_->offset_map()) {
      const auto component_id = entry.key();
      const auto offset = entry.val();
      component_id.destruct_func()(target + offset);
    }
  }
}

ConstView ArchetypeDataPage::operator[](const size_t index) const {
  MIRAGE_DCHECK(is_initialized());
  MIRAGE_DCHECK(index < size_);
  return {descriptor_.raw_ptr(), buffer_.ptr() + index * descriptor_->size()};
}

View ArchetypeDataPage::operator[](const size_t index) {
  MIRAGE_DCHECK(is_initialized());
  MIRAGE_DCHECK(index < size_);
  return {descriptor_.raw_ptr(), buffer_.ptr() + index * descriptor_->size()};
}

bool ArchetypeDataPage::is_initialized() const {
  return descriptor_ != nullptr;
}

const SharedDescriptor& ArchetypeDataPage::descriptor() const {
  return descriptor_;
}

size_t ArchetypeDataPage::capacity() const { return capacity_; }

size_t ArchetypeDataPage::size() const { return size_; }

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

ConstIterator::ConstIterator(const ArchetypeDataPage& page)
    : view_(page.descriptor().raw_ptr(), page.buffer().ptr()) {}

ConstIterator::ConstIterator(const Courier& courier)
    : view_(courier.descriptor().raw_ptr(), courier.buffer().ptr()) {}

ConstIterator::ConstIterator(std::nullptr_t) {}

ConstIterator& ConstIterator::operator=(std::nullptr_t) {
  view_.descriptor_ = nullptr;
  view_.view_ptr_ = nullptr;
  return *this;
}

ConstIterator::reference ConstIterator::operator*() const { return view_; }

ConstIterator::pointer ConstIterator::operator->() const { return view_; }

ConstIterator::reference ConstIterator::operator[](
    const difference_type diff) const {
  ConstView rv = view_;
  rv.view_ptr_ += diff * rv.descriptor_->size();
  return rv;
}

ConstIterator::iterator_type& ConstIterator::operator++() {
  view_.view_ptr_ += view_.descriptor_->size();
  return *this;
}

ConstIterator::iterator_type ConstIterator::operator++(int) {
  const iterator_type rv = *this;
  ++(*this);
  return rv;
}

ConstIterator::iterator_type& ConstIterator::operator--() {
  view_.view_ptr_ -= view_.descriptor_->size();
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
  return *this;
}

ConstIterator::iterator_type ConstIterator::operator-(
    difference_type diff) const {
  iterator_type rv = *this;
  rv -= diff;
  return rv;
}

ConstIterator::difference_type ConstIterator::operator-(
    const iterator_type& other) const {
  MIRAGE_DCHECK(view_.descriptor_ == other.view_.descriptor_);
  return (view_.view_ptr_ - other.view_.view_ptr_) / view_.descriptor_->size();
}

std::strong_ordering ConstIterator::operator<=>(
    const iterator_type& other) const {
  MIRAGE_DCHECK(view_.descriptor_ == other.view_.descriptor_);
  return view_.view_ptr_ <=> other.view_.view_ptr_;
}

bool ConstIterator::operator==(const iterator_type& other) const {
  return view_.descriptor_ == other.view_.descriptor_ &&
         view_.view_ptr_ == other.view_.view_ptr_;
}

ConstIterator::operator bool() const { return !is_null(); }

bool ConstIterator::is_null() const {
  return view_.descriptor_ == nullptr || view_.view_ptr_ == nullptr;
}

const void* View::TryGet(ComponentId id) const {
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

Iterator::Iterator(ArchetypeDataPage& page)
    : view_(page.descriptor().raw_ptr(), page.buffer().ptr()) {}

Iterator::Iterator(Courier& courier)
    : view_(courier.descriptor().raw_ptr(), courier.buffer().ptr()) {}

Iterator::Iterator(std::nullptr_t) {}

Iterator& Iterator::operator=(std::nullptr_t) {
  view_.descriptor_ = nullptr;
  view_.view_ptr_ = nullptr;
  return *this;
}

Iterator::reference Iterator::operator*() const { return view_; }

Iterator::pointer Iterator::operator->() const { return view_; }

Iterator::reference Iterator::operator[](difference_type diff) const {
  View rv = view_;
  rv.view_ptr_ += diff * rv.descriptor_->size();
  return rv;
}

Iterator::iterator_type& Iterator::operator++() {
  view_.view_ptr_ += view_.descriptor_->size();
  return *this;
}

Iterator::iterator_type Iterator::operator++(int) {
  iterator_type rv = *this;
  ++(*this);
  return rv;
}

Iterator::iterator_type& Iterator::operator--() {
  view_.view_ptr_ -= view_.descriptor_->size();
  return *this;
}

Iterator::iterator_type Iterator::operator--(int) {
  iterator_type rv = *this;
  --(*this);
  return rv;
}

Iterator::iterator_type& Iterator::operator+=(difference_type diff) {
  view_.view_ptr_ += diff * view_.descriptor_->size();
  return *this;
}

Iterator::iterator_type Iterator::operator+(difference_type diff) const {
  iterator_type rv = *this;
  rv += diff;
  return rv;
}

Iterator::iterator_type operator+(ptrdiff_t diff, const Iterator& iter) {
  return iter + diff;
}

Iterator::iterator_type& Iterator::operator-=(difference_type diff) {
  view_.view_ptr_ -= diff * view_.descriptor_->size();
  return *this;
}

Iterator::iterator_type Iterator::operator-(difference_type diff) const {
  iterator_type rv = *this;
  rv -= diff;
  return rv;
}

Iterator::difference_type Iterator::operator-(
    const iterator_type& other) const {
  MIRAGE_DCHECK(view_.descriptor_ == other.view_.descriptor_);
  return (view_.view_ptr_ - other.view_.view_ptr_) / view_.descriptor_->size();
}

std::strong_ordering Iterator::operator<=>(const iterator_type& other) const {
  MIRAGE_DCHECK(view_.descriptor_ == other.view_.descriptor_);
  return view_.view_ptr_ <=> other.view_.view_ptr_;
}

bool Iterator::operator==(const iterator_type& other) const {
  return view_.descriptor_ == other.view_.descriptor_ &&
         view_.view_ptr_ == other.view_.view_ptr_;
}

Iterator::operator bool() const { return !is_null(); }

bool Iterator::is_null() const {
  return view_.descriptor_ == nullptr || view_.view_ptr_ == nullptr;
}

Courier::~Courier() {
  if (is_null()) {
    return;
  }
  size_t size = this->size();
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

ConstIterator Courier::end() const { return begin() + size(); }

Iterator Courier::begin() { return Iterator(*this); }

Iterator Courier::end() { return begin() + size(); }

const SharedDescriptor& Courier::descriptor() const { return descriptor_; }

const Buffer& Courier::buffer() const { return buffer_; }

Buffer& Courier::buffer() { return buffer_; }

size_t Courier::size() const { return buffer_.size() / descriptor_->size(); }

Courier::Courier(ArchetypeDataPage& page,
                 const std::initializer_list<size_t> index_list)
    : descriptor_(page.descriptor_.Clone()),
      buffer_(Buffer(descriptor_->size() * index_list.size(),
                     descriptor_->align())) {
  std::byte* dest = buffer_.ptr();
  for (const size_t index : index_list) {
    std::byte* target = page.buffer().ptr() + index * descriptor_->size();
    for (const auto& entry : descriptor_->offset_map()) {
      const auto component_id = entry.key();
      const auto offset = entry.val();
      component_id.move_func()(target + offset, dest + offset);
    }
    dest += descriptor_->size();
  }
}
