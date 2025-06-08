#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE_PAGE_POOL
#define MIRAGE_ECS_ENTITY_ARCHETYPE_PAGE_POOL

#include <concepts>

#include "mirage_base/container/array.hpp"
#include "mirage_ecs/entity/archetype_data_page.hpp"

namespace mirage::ecs {

class ArchetypePagePool {
  template <std::move_constructible T>
  using Array = base::Array<T>;

 public:
  static constexpr size_t kPageSizeKB = 16 * 1024;

 private:
  Array<ArchetypeDataPage> pool_[5];
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE_PAGE_POOL
