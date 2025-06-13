#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE_PAGE_POOL
#define MIRAGE_ECS_ENTITY_ARCHETYPE_PAGE_POOL

#include <concepts>

#include "mirage_base/container/array.hpp"
#include "mirage_ecs/entity/archetype_data_page.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"

namespace mirage::ecs {

class ArchetypePagePool {
  template <std::move_constructible T>
  using Array = base::Array<T>;

 public:
  static constexpr size_t kPageSizeKB = 16;

  MIRAGE_ECS ArchetypePagePool() = default;
  MIRAGE_ECS ~ArchetypePagePool() = default;

  ArchetypePagePool(const ArchetypePagePool&) = delete;
  ArchetypePagePool& operator=(const ArchetypePagePool&) = delete;

  MIRAGE_ECS ArchetypePagePool(ArchetypePagePool&&) = default;
  MIRAGE_ECS ArchetypePagePool& operator=(ArchetypePagePool&&) = default;

  MIRAGE_ECS ArchetypeDataPage Allocate(size_t alignment);
  MIRAGE_ECS void Release(ArchetypeDataPage&& page);

 private:
  enum PoolIndex {
    kAlign8 = 0,
    kAlign4,
    kAlign2,
    kAlign1,
    kAlignOther,
    kMaxIndex,
  };

  MIRAGE_ECS PoolIndex GetPoolIndex(size_t alignment) const;

  Array<ArchetypeDataPage> pool_[PoolIndex::kMaxIndex];
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE_PAGE_POOL
