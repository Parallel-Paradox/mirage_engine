#include "mirage_ecs/util/aligned_buffer_pool.hpp"

#include "mirage_base/buffer/aligned_buffer.hpp"
#include "mirage_base/define/check.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

using PoolIndex = AlignedBufferPool::PoolIndex;

AlignedBuffer AlignedBufferPool::Allocate(size_t alignment) {
  PoolIndex index = GetPoolIndex(alignment);

  switch (index) {
    case kAlign8: {
      auto& pool = pool_[kAlign8];
      if (!pool.empty()) {
        return pool.Pop();
      }
      return AlignedBuffer(kBufferSize, 8);
    }
    case kAlignOther: {
      auto& pool = pool_[kAlignOther];
      auto size = pool.size();
      for (size_t i = 0; i < size; ++i) {
        if (pool[i].align() >= alignment) {
          return pool.SwapTake(i);
        }
      }
      return AlignedBuffer(kBufferSize, alignment);
    }
    default:
      NOT_REACHABLE;
  }
}

void AlignedBufferPool::Release(AlignedBuffer&& buffer) {
  MIRAGE_DCHECK(buffer.size() == kBufferSize);
  MIRAGE_DCHECK(buffer.align() >= 8);

  const PoolIndex index = GetPoolIndex(buffer.align());
  pool_[index].Emplace(std::move(buffer));
}

PoolIndex AlignedBufferPool::GetPoolIndex(size_t alignment) const {
  // Check if the alignment is a power of 2.
  MIRAGE_DCHECK((alignment != 0 && (alignment & (alignment - 1)) == 0));
  if (alignment <= 8) {
    return kAlign8;
  }
  return kAlignOther;
}
