#ifndef MIRAGE_ECS_ALIGNED_BUFFER_POOL
#define MIRAGE_ECS_ALIGNED_BUFFER_POOL

#include <concepts>

#include "mirage_base/buffer/aligned_buffer.hpp"
#include "mirage_base/container/array.hpp"
#include "mirage_base/util/constant.hpp"
#include "mirage_ecs/define/export.hpp"

namespace mirage::ecs {

class AlignedBufferPool {
  template <std::move_constructible T>
  using Array = base::Array<T>;

  using AlignedBuffer = base::AlignedBuffer;

 public:
  constexpr static size_t kBufferSize = 16 * base::kKB;
  constexpr static size_t kMinAlign = 8;

  enum PoolIndex {
    kAlign8 = 0,
    kAlignOther,
    kMaxIndex,
  };

  MIRAGE_ECS AlignedBufferPool() = default;
  MIRAGE_ECS ~AlignedBufferPool() = default;

  AlignedBufferPool(const AlignedBufferPool&) = delete;
  AlignedBufferPool& operator=(const AlignedBufferPool&) = delete;

  MIRAGE_ECS AlignedBufferPool(AlignedBufferPool&&) = default;
  MIRAGE_ECS AlignedBufferPool& operator=(AlignedBufferPool&&) = default;

  MIRAGE_ECS AlignedBuffer Allocate(size_t alignment);
  MIRAGE_ECS void Release(AlignedBuffer&& buffer);

 private:
  MIRAGE_ECS PoolIndex GetPoolIndex(size_t alignment) const;

  Array<AlignedBuffer> pool_[PoolIndex::kMaxIndex];
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ALIGNED_BUFFER_POOL
