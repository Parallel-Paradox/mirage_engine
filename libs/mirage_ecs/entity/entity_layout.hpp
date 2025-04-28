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

// TODO: Implement
class EntityLayout {
 public:
  struct ComponentInfo {
    size_t offset;
    void (*destructor)(void *);
  };
  using LayoutMap = base::HashMap<TypeId, ComponentInfo>;

  EntityLayout() = default;
  ~EntityLayout() = default;

  template <typename... Ts>
    requires IsComponentList<Ts...>
  static EntityLayout New();

  [[nodiscard]] size_t align() const;
  [[nodiscard]] size_t size() const;
  [[nodiscard]] const LayoutMap &layout_map() const;

 private:
  size_t align_{0};
  size_t size_{0};
  LayoutMap layout_map_;
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ENTITY_LAYOUT
