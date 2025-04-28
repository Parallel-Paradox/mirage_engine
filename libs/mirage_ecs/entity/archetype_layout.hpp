#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE_LAYOUT
#define MIRAGE_ECS_ENTITY_ARCHETYPE_LAYOUT

#include "mirage_base/container/hash_map.hpp"
#include "mirage_ecs/util/marker.hpp"
#include "mirage_ecs/util/type_id.hpp"
#include "mirage_ecs/util/type_set.hpp"

namespace mirage::ecs {

// TODO: Implement
class ArchetypeLayout {
 public:
  ArchetypeLayout() = default;
  ~ArchetypeLayout() = default;

  template <typename... Ts>
    requires IsComponentList<Ts...>
  static ArchetypeLayout New();

  size_t GetOffset(const TypeId &type_id) const;

  [[nodiscard]] const TypeSet &type_set() const;
  [[nodiscard]] size_t align() const;
  [[nodiscard]] size_t size() const;

 private:
  TypeSet type_set_;

  size_t align_{0};
  size_t size_{0};

  struct ComponentInfo {
    size_t offset;
    void (*destructor)(void *);
  };
  base::HashMap<TypeId, ComponentInfo> component_info_map_;
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE_LAYOUT
