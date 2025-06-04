#include "mirage_ecs/entity/archetype_data_page.hpp"

#include <cstddef>
#include <new>
#include <utility>

#include "mirage_base/buffer/aligned_buffer.hpp"
#include "mirage_base/define/check.hpp"
#include "mirage_base/wrap/box.hpp"
#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/component/component_id.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

using SharedDescriptor = ArchetypeDataPage::SharedDescriptor;
using Buffer = ArchetypeDataPage::Buffer;
using View = ArchetypeDataPage::View;
using Slice = ArchetypeDataPage::Slice;

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

bool ArchetypeDataPage::Push(Slice&& slice) {
  MIRAGE_DCHECK(is_initialized());
  if (size_ >= capacity_) {
    return false;
  }
  std::byte* dest = buffer_.ptr() + size_ * descriptor_->size();
  for (const auto& entry : descriptor_->offset_map()) {
    const auto component_id = entry.key();
    const auto offset = entry.val();

    void* component = slice.view().TryGet(component_id);
    MIRAGE_DCHECK(component != nullptr);
    component_id.move_func()(component, dest + offset);
  }
  size_ += 1;
  return true;
}

Slice ArchetypeDataPage::SwapPop(const size_t index) {
  MIRAGE_DCHECK(is_initialized());
  MIRAGE_DCHECK(index < size_);
  auto rv = Slice(*this, index);
  size_ -= 1;
  std::byte* last = buffer_.ptr() + size_ * descriptor_->size();
  std::byte* dest = buffer_.ptr() + index * descriptor_->size();
  for (const auto& entry : descriptor_->offset_map()) {
    const auto component_id = entry.key();
    const auto offset = entry.val();

    component_id.move_func()(last + offset, dest + offset);
    component_id.destruct_func()(last + offset);
  }
  return rv;
}

void ArchetypeDataPage::SwapRemove(const size_t index) {
  MIRAGE_DCHECK(is_initialized());
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

View ArchetypeDataPage::operator[](size_t index) const {
  MIRAGE_DCHECK(is_initialized());
  MIRAGE_DCHECK(index < size_);
  return {*this, index};
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

View::View(const ArchetypeDataPage& page, const size_t index)
    : descriptor_(page.descriptor().raw_ptr()),
      view_ptr_(page.buffer().ptr() + index * page.descriptor_->size()) {
  MIRAGE_DCHECK(index < page.size());
}

View::View(const Slice& slice)
    : descriptor_(slice.descriptor().raw_ptr()), view_ptr_(slice.slice_ptr()) {}

View::operator bool() const { return !is_null(); }

bool View::is_null() const {
  return descriptor_ == nullptr || view_ptr_ == nullptr;
}

void* View::TryGet(ComponentId id) const {
  const auto& offset_map = descriptor_->offset_map();
  auto iter = offset_map.TryFind(id);
  if (iter == offset_map.end()) {
    return nullptr;
  }
  return view_ptr_ + iter->val();
}

Slice::Slice(ArchetypeDataPage& page, size_t index)
    : descriptor_(page.descriptor_.Clone()),
      slice_ptr_(static_cast<std::byte*>(::operator new[](
          descriptor_->size(), std::align_val_t{descriptor_->align()}))) {
  std::byte* target = page.buffer().ptr() + index * descriptor_->size();
  for (const auto& entry : descriptor_->offset_map()) {
    const auto component_id = entry.key();
    const auto offset = entry.val();
    component_id.move_func()(target + offset, slice_ptr_ + offset);
  }
}

Slice::~Slice() {
  if (is_null()) {
    return;
  }
  for (const auto& entry : descriptor_->offset_map()) {
    const auto component_id = entry.key();
    const auto offset = entry.val();
    component_id.destruct_func()(slice_ptr_ + offset);
  }
  ::operator delete[](slice_ptr_, std::align_val_t{descriptor_->align()});
  descriptor_ = nullptr;
  slice_ptr_ = nullptr;
}

Slice::Slice(Slice&& other) noexcept
    : descriptor_(std::move(other.descriptor_)), slice_ptr_(other.slice_ptr_) {
  other.descriptor_ = nullptr;
  other.slice_ptr_ = nullptr;
}

Slice& Slice::operator=(Slice&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~Slice();
  new (this) Slice(std::move(other));
  return *this;
}

Slice::operator bool() const { return !is_null(); }

bool Slice::is_null() const {
  return descriptor_ == nullptr || slice_ptr_ == nullptr;
}

View Slice::view() const { return View(*this); }

const SharedDescriptor& Slice::descriptor() const { return descriptor_; }

std::byte* Slice::slice_ptr() const { return slice_ptr_; }
