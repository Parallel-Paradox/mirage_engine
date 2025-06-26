#include "mirage_ecs/util/aligned_buffer_pool.hpp"

#include "mirage_base/buffer/aligned_buffer.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

using PoolIndex = AlignedBufferPool::PoolIndex;

AlignedBuffer AlignedBufferPool::Allocate(size_t alignment) {
  PoolIndex index = GetPoolIndex(alignment);

  if (index == kAlignOther) {
    auto& pool = pool_[kAlignOther];
    auto size = pool.size();
    for (size_t i = 0; i < size; ++i) {
      if (pool[i].align() >= alignment) {
        return pool.SwapTake(i);
      }
    }
    return AlignedBuffer(kBufferSize, alignment);
  }

  int32_t index_num = static_cast<int32_t>(index);
  while (index_num < kMaxIndex && pool_[index_num].empty()) {
    ++index_num;
  }
  if (index_num < kMaxIndex) {
    return pool_[index_num].Pop();
  }
  return AlignedBuffer(kBufferSize, alignment);
}

void AlignedBufferPool::Release(AlignedBuffer&& buffer) {
  MIRAGE_DCHECK(buffer.size() == kBufferSize);
  const PoolIndex index = GetPoolIndex(buffer.align());
  pool_[index].Emplace(std::move(buffer));
}

PoolIndex AlignedBufferPool::GetPoolIndex(size_t alignment) const {
  // Check if the alignment is a power of 2.
  MIRAGE_DCHECK((alignment != 0 && (alignment & (alignment - 1)) == 0));
  switch (alignment) {
    case 8:
      return kAlign8;
    case 4:
      return kAlign4;
    case 2:
      return kAlign2;
    case 1:
      return kAlign1;
    default:
      return kAlignOther;
  }
}
