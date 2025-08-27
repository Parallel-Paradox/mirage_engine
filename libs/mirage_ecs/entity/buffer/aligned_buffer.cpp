#include "mirage_ecs/entity/buffer/aligned_buffer.hpp"

#include <cstddef>
#include <new>
#include <utility>

#include "mirage_base/define/check.hpp"
#include "mirage_base/util/math.hpp"

using namespace mirage::ecs;

AlignedBuffer::AlignedBuffer(const size_t size, size_t align)
    : buffer_ptr_(static_cast<std::byte*>(
          ::operator new[](size, std::align_val_t{align}))),
      size_(size),
      align_(align) {
  MIRAGE_DCHECK(size > 0);
  MIRAGE_DCHECK(base::IsPowerOfTwo(align));
}

AlignedBuffer::~AlignedBuffer() {
  if (buffer_ptr_) {
    ::operator delete[](buffer_ptr_, std::align_val_t{align_});
  }
  buffer_ptr_ = nullptr;
  size_ = 0;
  align_ = 0;
}

AlignedBuffer::AlignedBuffer(AlignedBuffer&& other) noexcept
    : buffer_ptr_(other.buffer_ptr_), size_(other.size_), align_(other.align_) {
  other.buffer_ptr_ = nullptr;
  other.size_ = 0;
  other.align_ = 0;
}

AlignedBuffer& AlignedBuffer::operator=(AlignedBuffer&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~AlignedBuffer();
  new (this) AlignedBuffer(std::move(other));
  return *this;
}

std::byte* AlignedBuffer::ptr() { return buffer_ptr_; }

const std::byte* AlignedBuffer::ptr() const { return buffer_ptr_; }

size_t AlignedBuffer::size() const { return size_; }

size_t AlignedBuffer::align() const { return align_; }
