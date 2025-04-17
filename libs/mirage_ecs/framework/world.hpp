#ifndef MIRAGE_ECS_FRAMEWORK_WORLD
#define MIRAGE_ECS_FRAMEWORK_WORLD

#include "mirage_base/auto_ptr/owned.hpp"
#include "mirage_base/container/hash_map.hpp"
#include "mirage_ecs/define.hpp"
#include "mirage_ecs/entity/archetype.hpp"
#include "mirage_ecs/util/marker.hpp"
#include "mirage_ecs/util/type_id.hpp"
#include "mirage_ecs/util/type_set.hpp"

namespace mirage::ecs {

class World {
 public:
  World() = default;
  ~World() = default;

 private:
  base::HashMap<TypeId, base::Owned<Resource>> resource_map_;
  // base::HashMap<TypeSet, Archetype> archetype_map_;
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_FRAMEWORK_WORLD
