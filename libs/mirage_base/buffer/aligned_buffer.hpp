#ifndef MIRAGE_BASE_BUFFER_ALIGNED_BUFFER
#define MIRAGE_BASE_BUFFER_ALIGNED_BUFFER

#include <cstddef>

#include "mirage_base/define/export.hpp"

namespace mirage::base {

class MIRAGE_BASE AlignedBuffer {
 public:
  AlignedBuffer() = delete;
  AlignedBuffer(size_t size, size_t align);

  ~AlignedBuffer();

  AlignedBuffer(const AlignedBuffer&) = delete;
  AlignedBuffer& operator=(const AlignedBuffer&) = delete;

  AlignedBuffer(AlignedBuffer&&) noexcept;
  AlignedBuffer& operator=(AlignedBuffer&&) noexcept;

  [[nodiscard]] std::byte* ptr() const;
  [[nodiscard]] size_t size() const;
  [[nodiscard]] size_t align() const;

 private:
  std::byte* buffer_ptr_{nullptr};
  size_t size_{0};
  size_t align_{0};
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_BUFFER_ALIGNED_BUFFER
