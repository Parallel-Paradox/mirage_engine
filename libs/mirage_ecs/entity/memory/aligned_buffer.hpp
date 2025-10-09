#ifndef MIRAGE_ECS_ALIGNED_BUFFER
#define MIRAGE_ECS_ALIGNED_BUFFER

#include <cstddef>

#include "mirage_ecs/define/export.hpp"

namespace mirage::ecs {

class MIRAGE_ECS AlignedBuffer {
 public:
  AlignedBuffer() = default;
  AlignedBuffer(size_t size, size_t align);

  ~AlignedBuffer();

  AlignedBuffer(const AlignedBuffer&) = delete;
  AlignedBuffer& operator=(const AlignedBuffer&) = delete;

  AlignedBuffer(AlignedBuffer&&) noexcept;
  AlignedBuffer& operator=(AlignedBuffer&&) noexcept;

  [[nodiscard]] std::byte* ptr();
  [[nodiscard]] const std::byte* ptr() const;
  [[nodiscard]] size_t size() const;
  [[nodiscard]] size_t align() const;

 private:
  std::byte* ptr_{nullptr};
  size_t size_{0};
  size_t align_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ALIGNED_BUFFER
