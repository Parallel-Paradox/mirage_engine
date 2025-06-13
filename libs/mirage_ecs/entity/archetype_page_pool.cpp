#include "mirage_ecs/entity/archetype_page_pool.hpp"

#include "mirage_base/define/check.hpp"
#include "mirage_ecs/entity/archetype_data_page.hpp"

using namespace mirage::ecs;

ArchetypeDataPage ArchetypePagePool::Allocate(size_t alignment) {
  const PoolIndex index = GetPoolIndex(alignment);
  if (!pool_[index].empty()) {
    return pool_[index].Pop();
  }
  return ArchetypeDataPage(kPageSizeKB * 1024, alignment);
}

void ArchetypePagePool::Release(ArchetypeDataPage&& page) {
  MIRAGE_DCHECK(page.buffer().size() == kPageSizeKB * 1024);
  page.Reset();
  const PoolIndex index = GetPoolIndex(page.buffer().align());
  pool_[index].Emplace(std::move(page));
}

ArchetypePagePool::PoolIndex ArchetypePagePool::GetPoolIndex(
    size_t alignment) const {
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
