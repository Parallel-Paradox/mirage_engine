#ifndef MIRAGE_ECS_ENTITY_COMPONENT_FUNC_TABLE
#define MIRAGE_ECS_ENTITY_COMPONENT_FUNC_TABLE

#include "mirage_ecs/util/marker.hpp"

namespace mirage::ecs {

template <IsComponent T>
void DestructComponent(void *component_ptr) {
  static_cast<T *>(component_ptr)->~T();
}

template <IsComponent T>
void SwapComponent(void *lhs, void *rhs) {
  T *typed_lhs = static_cast<T *>(lhs);
  T *typed_rhs = static_cast<T *>(rhs);

  T tmp = std::move(*typed_lhs);
  *typed_lhs = std::move(*typed_rhs);
  *typed_rhs = std::move(tmp);
}

struct ComponentFuncTable {
  void (*destruct)(void *){nullptr};
  void (*swap)(void *, void *){nullptr};

  template <IsComponent T>
  static ComponentFuncTable Of() {
    return ComponentFuncTable{
        .destruct = DestructComponent<T>,
        .swap = SwapComponent<T>,
    };
  }
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_COMPONENT_FUNC_TABLE
