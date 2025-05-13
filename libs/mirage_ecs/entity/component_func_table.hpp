#ifndef MIRAGE_ECS_ENTITY_COMPONENT_FUNC_TABLE
#define MIRAGE_ECS_ENTITY_COMPONENT_FUNC_TABLE

#include "mirage_ecs/util/marker.hpp"

namespace mirage::ecs {

template <IsComponent T>
void DestructComponent(void *component_ptr) {
  static_cast<T *>(component_ptr)->~T();
}

template <IsComponent T>
void MoveComponent(void *target, void *destination) {
  T *typed_target = static_cast<T *>(target);
  T *typed_destination = static_cast<T *>(destination);
  *typed_destination = std::move(*typed_target);
}

struct ComponentFuncTable {
  void (*destruct)(void *component_ptr){nullptr};
  void (*move)(void *target, void *destination){nullptr};

  template <IsComponent T>
  static ComponentFuncTable Of() {
    return ComponentFuncTable{
        .destruct = DestructComponent<T>,
        .move = MoveComponent<T>,
    };
  }
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_COMPONENT_FUNC_TABLE
