#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE
#define MIRAGE_ECS_ENTITY_ARCHETYPE

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/container/hash_map.hpp"
#include "mirage_ecs/define.hpp"
#include "mirage_ecs/entity/data_chunk.hpp"
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

  template <typename... Ts>
    requires IsComponentList<Ts...>
  static Archetype New();
  explicit MIRAGE_ECS Archetype(TypeSet &&type_set);

  [[nodiscard]] MIRAGE_ECS const TypeSet &type_set() const;
  [[nodiscard]] MIRAGE_ECS const base::SharedLocal<DataChunk::Header> &
  data_chunk_header() const;

 private:
  // TODO
  TypeSet type_set_;
  base::SharedLocal<DataChunk::Header> data_chunk_header_{nullptr};
};

template <typename... Ts>
  requires IsComponentList<Ts...>
Archetype Archetype::New() {
  return Archetype(TypeSet::New<Ts...>());
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE
