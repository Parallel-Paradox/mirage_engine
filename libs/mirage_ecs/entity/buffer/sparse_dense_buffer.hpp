#ifndef MIRAGE_ECS_SPARSE_DENSE_BUFFER
#define MIRAGE_ECS_SPARSE_DENSE_BUFFER

#include <cstddef>
#include <cstdint>

#include "mirage_base/buffer/aligned_buffer.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/buffer/aligned_buffer_pool.hpp"

namespace mirage::ecs {

using SparseId = size_t;
constexpr static inline SparseId kInvalidSparseId = SIZE_MAX;

using DenseId = size_t;
constexpr static inline DenseId kInvalidDenseId = SIZE_MAX;

class MIRAGE_ECS DenseBuffer {
  using Buffer = base::AlignedBuffer;

 public:
  constexpr static size_t kMaxBufferSize = AlignedBufferPool::kBufferSize;
  constexpr static size_t kMinAlign = alignof(SparseId);

  DenseBuffer() = default;
  explicit DenseBuffer(Buffer&& buffer);
  ~DenseBuffer();

  DenseBuffer(const DenseBuffer&) = delete;
  DenseBuffer& operator=(const DenseBuffer&) = delete;

  DenseBuffer(DenseBuffer&&) noexcept;
  DenseBuffer& operator=(DenseBuffer&&) noexcept;

  SparseId const& operator[](uint16_t index) const;
  SparseId& operator[](uint16_t index);

  void Push(SparseId sparse_id);
  void RemoveTail();

  uint16_t size() const;
  uint16_t capacity() const;

 private:
  Buffer buffer_;
  uint16_t size_{0};
  uint16_t capacity_{0};
};

class MIRAGE_ECS SparseBuffer {
  using Buffer = base::AlignedBuffer;

 public:
  constexpr static size_t kMaxBufferSize = AlignedBufferPool::kBufferSize;
  constexpr static size_t kMinAlign = alignof(DenseId);

  SparseBuffer() = default;
  explicit SparseBuffer(Buffer&& buffer);
  ~SparseBuffer();

  SparseBuffer(const SparseBuffer&) = delete;
  SparseBuffer& operator=(const SparseBuffer&) = delete;

  SparseBuffer(SparseBuffer&&) noexcept;
  SparseBuffer& operator=(SparseBuffer&&) noexcept;

  DenseId const& operator[](uint16_t index) const;
  DenseId& operator[](uint16_t index);

  [[nodiscard]] uint16_t FillHole(DenseId dense_id);
  DenseId Remove(uint16_t index);

  uint16_t size() const;
  uint16_t hole_cnt() const;
  uint16_t capacity() const;

 private:
  Buffer buffer_;
  uint16_t size_{0};
  uint16_t hole_cnt_{0};
  uint16_t capacity_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SPARSE_DENSE_BUFFER
