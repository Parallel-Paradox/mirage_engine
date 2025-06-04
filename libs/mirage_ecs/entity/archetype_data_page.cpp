#include "mirage_ecs/entity/archetype_data_page.hpp"

#include <cstddef>
#include <new>
#include <utility>

#include "mirage_base/buffer/aligned_buffer.hpp"
#include "mirage_base/define/check.hpp"
#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/component/component_id.hpp"

using namespace mirage::ecs;

using SharedDescriptor = ArchetypeDataPage::SharedDescriptor;
using Buffer = ArchetypeDataPage::Buffer;
using View = ArchetypeDataPage::View;
using Slice = ArchetypeDataPage::Slice;

ArchetypeDataPage::ArchetypeDataPage(size_t buffer_size, size_t align)
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
  // TODO
  return false;
}

bool ArchetypeDataPage::Push(Slice&& slice) {
  MIRAGE_DCHECK(is_initialized());
  // TODO
  return false;
}

Slice ArchetypeDataPage::SwapPop(size_t index) {
  MIRAGE_DCHECK(is_initialized());
  MIRAGE_DCHECK(index < size_);
  // TODO
  return Slice();
}

void ArchetypeDataPage::SwapRemove(size_t index) {
  MIRAGE_DCHECK(is_initialized());
  MIRAGE_DCHECK(index < size_);
  // TODO
}

void ArchetypeDataPage::Clear() {
  if (!is_initialized()) {
    return;
  }
  for (size_t i = 0; i < size_; ++i) {
    std::byte* begin = buffer_.ptr() + i * descriptor_->size();
    for (const auto& entry : descriptor_->offset_map()) {
      entry.key().destruct_func()(begin + entry.val());
    }
  }
}

View ArchetypeDataPage::operator[](size_t index) {
  MIRAGE_DCHECK(is_initialized());
  MIRAGE_DCHECK(index < size_);
  // TODO
  return View();
}

const View ArchetypeDataPage::operator[](size_t index) const {
  MIRAGE_DCHECK(is_initialized());
  MIRAGE_DCHECK(index < size_);
  // TODO
  return View();
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

View::View(const ArchetypeDataPage& page, size_t index)
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

void* View::TryGetImpl(ComponentId id) const {
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
  std::byte* begin = page.buffer().ptr() + index * descriptor_->size();
  for (const auto& entry : descriptor_->offset_map()) {
    entry.key().move_func()(begin + entry.val(), slice_ptr_ + entry.val());
  }
}

Slice::~Slice() {
  if (is_null()) {
    return;
  }
  for (const auto& entry : descriptor_->offset_map()) {
    entry.key().destruct_func()(slice_ptr_ + entry.val());
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

View Slice::view() { return View(*this); }

const View Slice::view() const { return View(*this); }

const SharedDescriptor& Slice::descriptor() const { return descriptor_; }

std::byte* Slice::slice_ptr() const { return slice_ptr_; }
