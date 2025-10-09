#include "mirage_ecs/entity/buffer/aligned_buffer_pool.hpp"

#include "mirage_base/define/check.hpp"
#include "mirage_base/util/math.hpp"
#include "mirage_ecs/entity/memory/aligned_buffer.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

using PoolIndex = AlignedBufferPool::PoolIndex;

AlignedBuffer AlignedBufferPool::Allocate(const size_t alignment) {
  switch (const PoolIndex index = GetPoolIndex(alignment)) {
    case kAlign8: {
      size_t index_num = static_cast<size_t>(index);
      while (pool_[index_num].empty() && index_num < kMaxIndex) {
        ++index_num;
      }
      return index_num < kMaxIndex ? pool_[index_num].Pop()
                                   : AlignedBuffer(kBufferSize, 8);
    }
    case kAlignOther: {
      auto& pool = pool_[kAlignOther];
      const auto size = pool.size();
      for (size_t i = 0; i < size; ++i) {
        if (pool[i].align() >= alignment) {
          return pool.SwapTake(i);
        }
      }
      return AlignedBuffer(kBufferSize, alignment);
    }
    default: {
      NOT_REACHABLE;
      return AlignedBuffer();  // NOLINT: Return value for all branches.
    }
  }
}

void AlignedBufferPool::Release(AlignedBuffer&& buffer) {
  MIRAGE_DCHECK(buffer.size() == kBufferSize);
  MIRAGE_DCHECK(buffer.align() >= kMinAlign);

  const PoolIndex index = GetPoolIndex(buffer.align());
  pool_[index].Emplace(std::move(buffer));
}

PoolIndex AlignedBufferPool::GetPoolIndex(const size_t alignment) {
  MIRAGE_DCHECK(IsPowerOfTwo(alignment));
  if (alignment <= kMinAlign) {
    return kAlign8;
  }
  return kAlignOther;
}
