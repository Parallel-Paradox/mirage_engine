#ifndef MIRAGE_ECS_ENTITY_ENTITY_LAYOUT
#define MIRAGE_ECS_ENTITY_ENTITY_LAYOUT

#include "mirage_base/container/hash_map.hpp"
#include "mirage_ecs/util/marker.hpp"
#include "mirage_ecs/util/type_id.hpp"
#include "mirage_ecs/util/type_set.hpp"

namespace mirage::ecs {

template <IsComponent T>
void DestroyComponent(void *component_ptr) {
  ((T *)component_ptr)->~T();  // NOLINT: Avoid virtual table in component
}

class EntityLayout {
 public:
  struct ComponentMeta {
    size_t offset;
    void (*destructor)(void *);
  };
  using ComponentMetaMap = base::HashMap<TypeId, ComponentMeta>;

  ~EntityLayout() = default;

  EntityLayout(const EntityLayout &) = delete;
  EntityLayout &operator=(const EntityLayout &) = delete;

  EntityLayout(EntityLayout &&) = default;
  EntityLayout &operator=(EntityLayout &&) = default;

  template <IsComponent... Ts>
  static EntityLayout New();

  [[nodiscard]] size_t align() const;
  [[nodiscard]] size_t size() const;
  [[nodiscard]] const TypeSet &component_type_set() const;
  [[nodiscard]] const ComponentMetaMap &component_meta_map() const;

 private:
  EntityLayout() = default;

  void set_component_type_set(TypeSet &&type_set);

  size_t align_{0};
  size_t size_{0};
  TypeSet component_type_set_;
  ComponentMetaMap component_meta_map_;
};

template <IsComponent... Ts>
EntityLayout EntityLayout::New() {
  EntityLayout layout;
  layout.set_component_type_set(TypeSet::New<Ts...>());

  ((layout.component_meta_map_[TypeId::Of<Ts>()].destructor =
        DestroyComponent<Ts>),
   ...);
  return layout;
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ENTITY_LAYOUT
