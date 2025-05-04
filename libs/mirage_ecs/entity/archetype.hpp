#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE
#define MIRAGE_ECS_ENTITY_ARCHETYPE

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/container/hash_map.hpp"
#include "mirage_ecs/define.hpp"
#include "mirage_ecs/entity/entity_chunk.hpp"
#include "mirage_ecs/util/marker.hpp"
#include "mirage_ecs/util/type_id.hpp"
#include "mirage_ecs/util/type_set.hpp"

namespace mirage::ecs {

class Archetype {
 public:
  MIRAGE_ECS Archetype() = delete;
  MIRAGE_ECS ~Archetype() = default;

  MIRAGE_ECS Archetype(const Archetype &) = delete;
  MIRAGE_ECS Archetype &operator=(const Archetype &) = delete;

  MIRAGE_ECS Archetype(Archetype &&other) noexcept = default;
  MIRAGE_ECS Archetype &operator=(Archetype &&other) noexcept = default;

  template <IsComponent... Ts>
  static Archetype New();

 private:
  // TODO
};

template <IsComponent... Ts>
Archetype Archetype::New() {
  // TODO
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE
