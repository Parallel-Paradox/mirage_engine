#ifndef MIRAGE_ECS_ENTITY_ENTITY_DESCRIPTOR
#define MIRAGE_ECS_ENTITY_ENTITY_DESCRIPTOR

#include <concepts>

#include "mirage_base/container/hash_map.hpp"
#include "mirage_ecs/component/component_id.hpp"
#include "mirage_ecs/util/marker.hpp"
#include "mirage_ecs/util/type_set.hpp"

namespace mirage::ecs {

class EntityDescriptor {
  using TypeId = base::TypeId;

 public:
  using OffsetMap = base::HashMap<ComponentId, size_t>;

  MIRAGE_ECS EntityDescriptor(base::Array<ComponentId> component_id_array);
  MIRAGE_ECS ~EntityDescriptor() = default;

  EntityDescriptor(const EntityDescriptor &) = delete;
  EntityDescriptor &operator=(const EntityDescriptor &) = delete;

  MIRAGE_ECS EntityDescriptor(EntityDescriptor &&) = default;
  MIRAGE_ECS EntityDescriptor &operator=(EntityDescriptor &&) = default;

  template <IsComponent... Ts>
  static EntityDescriptor New();

  [[nodiscard]] MIRAGE_ECS size_t align() const;
  [[nodiscard]] MIRAGE_ECS size_t size() const;
  [[nodiscard]] MIRAGE_ECS const OffsetMap &offset_map() const;
  [[nodiscard]] MIRAGE_ECS const TypeSet &type_set() const;

 private:
  size_t align_{0};
  size_t size_{0};
  OffsetMap offset_map_{};
  TypeSet type_set_{};
};

template <IsComponent... Ts>
EntityDescriptor EntityDescriptor::New() {
  base::Array<ComponentId> component_id_array;
  component_id_array.Reserve(sizeof...(Ts));
  (component_id_array.Push(ComponentId::Of<Ts>()), ...);
  return EntityDescriptor(std::move(component_id_array));
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ENTITY_DESCRIPTOR
