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

  ArchetypePagePool() = default;
  ~ArchetypePagePool() = default;

  ArchetypePagePool(const ArchetypePagePool&) = delete;
  ArchetypePagePool& operator=(const ArchetypePagePool&) = delete;

  ArchetypePagePool(ArchetypePagePool&&) = default;
  ArchetypePagePool& operator=(ArchetypePagePool&&) = default;

  ArchetypeDataPage Allocate(size_t alignment);
  void Release(ArchetypeDataPage&& page);

 private:
  enum PoolIndex {
    kAlign8 = 0,
    kAlign4,
    kAlign2,
    kAlign1,
    kAlignOther,
    kMaxIndex,
  };

  PoolIndex GetPoolIndex(size_t alignment) const;

  Array<ArchetypeDataPage> pool_[PoolIndex::kMaxIndex];
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE_PAGE_POOL
