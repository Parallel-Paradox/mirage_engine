#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE
#define MIRAGE_ECS_ENTITY_ARCHETYPE

#include "mirage_base/container/hash_map.hpp"
#include "mirage_ecs/define.hpp"
#include "mirage_ecs/util/type_id.hpp"
#include "mirage_ecs/util/type_set.hpp"

namespace mirage::ecs {

class Archetype {
 public:
  MIRAGE_ECS Archetype() = default;
  MIRAGE_ECS ~Archetype() = default;

  MIRAGE_ECS Archetype(const Archetype &) = delete;
  MIRAGE_ECS Archetype &operator=(const Archetype &) = delete;

  MIRAGE_ECS Archetype(Archetype &&other) noexcept = default;
  MIRAGE_ECS Archetype &operator=(Archetype &&other) noexcept = default;

  template <typename... Ts>
  static Archetype New();
  explicit MIRAGE_ECS Archetype(TypeSet &&type_set);

  [[nodiscard]] MIRAGE_ECS const TypeSet &type_set() const;
  [[nodiscard]] MIRAGE_ECS size_t entity_align() const;
  [[nodiscard]] MIRAGE_ECS size_t entity_size() const;
  [[nodiscard]] MIRAGE_ECS const base::HashMap<TypeId, size_t> &
  type_addr_offset_map() const;

 private:
  // TODO
  TypeSet type_set_;
  size_t entity_align_{0};
  size_t entity_size_{0};
  base::HashMap<TypeId, size_t> type_addr_offset_map_;
};

template <typename... Ts>
Archetype Archetype::New() {
  return Archetype(TypeSet::New<Ts...>());
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE
