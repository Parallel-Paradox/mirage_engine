#ifndef MIRAGE_ECS_ENTITY_ENTITY_DESCRIPTOR
#define MIRAGE_ECS_ENTITY_ENTITY_DESCRIPTOR

#include <concepts>

#include "mirage_base/container/hash_map.hpp"
#include "mirage_base/util/type_id.hpp"
#include "mirage_ecs/util/marker.hpp"
#include "mirage_ecs/util/type_set.hpp"

namespace mirage::ecs {

class EntityDescriptor {
 public:
  struct ComponentMeta {
    size_t offset;
    void (*destruct_func)(void *ptr){nullptr};
    void (*move_func)(void *target, void *destination){nullptr};
  };
  using TypeId = base::TypeId;
  using ComponentMetaMap = base::HashMap<TypeId, ComponentMeta>;

  MIRAGE_ECS ~EntityDescriptor() = default;

  EntityDescriptor(const EntityDescriptor &) = delete;
  EntityDescriptor &operator=(const EntityDescriptor &) = delete;

  MIRAGE_ECS EntityDescriptor(EntityDescriptor &&) = default;
  MIRAGE_ECS EntityDescriptor &operator=(EntityDescriptor &&) = default;

  template <IsComponent... Ts>
  static EntityDescriptor New();

  [[nodiscard]] MIRAGE_ECS size_t align() const;
  [[nodiscard]] MIRAGE_ECS size_t size() const;
  [[nodiscard]] MIRAGE_ECS const TypeSet &component_type_set() const;
  [[nodiscard]] MIRAGE_ECS const ComponentMetaMap &component_meta_map() const;

 private:
  MIRAGE_ECS EntityDescriptor() = default;

  MIRAGE_ECS void InitializeLayout(TypeSet &&type_set);

  size_t align_{0};
  size_t size_{0};
  TypeSet component_type_set_{};
  ComponentMetaMap component_meta_map_{};
};

template <typename T>
void DestructFunc(void *ptr) {
  static_cast<T *>(ptr)->~T();
}

template <std::move_constructible T>
void MoveFunc(void *target, void *destination) {
  T *typed_target = static_cast<T *>(target);
  T *typed_destination = static_cast<T *>(destination);
  *typed_destination = std::move(*typed_target);
}

template <IsComponent... Ts>
EntityDescriptor EntityDescriptor::New() {
  EntityDescriptor layout;
  layout.InitializeLayout(TypeSet::New<Ts...>());

  ((layout.component_meta_map_[base::TypeId::Of<Ts>()].destruct_func =
        DestructFunc<Ts>),
   ...);
  ((layout.component_meta_map_[base::TypeId::Of<Ts>()].move_func =
        MoveFunc<Ts>),
   ...);

  return layout;
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ENTITY_DESCRIPTOR
