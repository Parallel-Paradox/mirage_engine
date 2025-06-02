#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE
#define MIRAGE_ECS_ENTITY_ARCHETYPE

#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/util/marker.hpp"

namespace mirage::ecs {

class Archetype {
 public:
  class Iterator;

  Archetype() = delete;
  MIRAGE_ECS ~Archetype() = default;

  Archetype(const Archetype &) = delete;
  Archetype &operator=(const Archetype &) = delete;

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
