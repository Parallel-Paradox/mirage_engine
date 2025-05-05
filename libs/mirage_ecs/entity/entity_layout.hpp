#ifndef MIRAGE_ECS_ENTITY_ENTITY_LAYOUT
#define MIRAGE_ECS_ENTITY_ENTITY_LAYOUT

#include "mirage_base/container/hash_map.hpp"
#include "mirage_ecs/entity/component_func_table.hpp"
#include "mirage_ecs/util/marker.hpp"
#include "mirage_ecs/util/type_id.hpp"
#include "mirage_ecs/util/type_set.hpp"

namespace mirage::ecs {

class EntityLayout {
 public:
  struct ComponentMeta {
    size_t offset;
    ComponentFuncTable func_table;
  };
  using ComponentMetaMap = base::HashMap<TypeId, ComponentMeta>;

  MIRAGE_ECS ~EntityLayout() = default;

  EntityLayout(const EntityLayout &) = delete;
  EntityLayout &operator=(const EntityLayout &) = delete;

  MIRAGE_ECS EntityLayout(EntityLayout &&) = default;
  MIRAGE_ECS EntityLayout &operator=(EntityLayout &&) = default;

  template <IsComponent... Ts>
  static EntityLayout New();

  [[nodiscard]] MIRAGE_ECS size_t align() const;
  [[nodiscard]] MIRAGE_ECS size_t size() const;
  [[nodiscard]] MIRAGE_ECS const TypeSet &component_type_set() const;
  [[nodiscard]] MIRAGE_ECS const ComponentMetaMap &component_meta_map() const;

 private:
  MIRAGE_ECS EntityLayout() = default;

  MIRAGE_ECS void set_component_type_set(TypeSet &&type_set);

  size_t align_{0};
  size_t size_{0};
  TypeSet component_type_set_{};
  ComponentMetaMap component_meta_map_{};
};

template <IsComponent... Ts>
EntityLayout EntityLayout::New() {
  EntityLayout layout;
  layout.set_component_type_set(TypeSet::New<Ts...>());

  ((layout.component_meta_map_[TypeId::Of<Ts>()].func_table =
        ComponentFuncTable::Of<Ts>()),
   ...);

  return layout;
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ENTITY_LAYOUT
