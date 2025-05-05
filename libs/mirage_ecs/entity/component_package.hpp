#ifndef MIRAGE_ECS_ENTITY_COMPONENT_PACKAGE
#define MIRAGE_ECS_ENTITY_COMPONENT_PACKAGE

#include <cstddef>

#include "mirage_ecs/entity/component_func_table.hpp"
#include "mirage_ecs/util/marker.hpp"
#include "mirage_ecs/util/type_id.hpp"
#include "mirage_ecs/util/type_set.hpp"

namespace mirage::ecs {

class ComponentData {
 public:
  ComponentData() = delete;
  MIRAGE_ECS ~ComponentData();

  ComponentData(const ComponentData &) = delete;
  ComponentData &operator=(const ComponentData &) = delete;

  MIRAGE_ECS ComponentData(ComponentData &&other) noexcept;
  MIRAGE_ECS ComponentData &operator=(ComponentData &&other) noexcept;

  template <IsComponent T>
  static ComponentData New(T component);

  template <IsComponent T>
  T Unwrap() const;

  [[nodiscard]] MIRAGE_ECS void *raw_ptr() const;
  [[nodiscard]] MIRAGE_ECS const TypeId &type_id() const;

 private:
  ComponentData(void *raw_ptr, TypeId type_id, void (*destroy_func)(void *));

  void *raw_ptr_{nullptr};

  TypeId type_id_;
  void (*destroy_func_)(void *){nullptr};
};

class ComponentPackage {
 public:
 private:
  TypeSet type_set_;
  // TODO
};

template <IsComponent T>
ComponentData ComponentData::New(T component) {
  return ComponentData(static_cast<void *>(new T(std::move(component))),
                       TypeId::Of<T>(),
                       [](void *raw_ptr) { delete static_cast<T *>(raw_ptr); });
}

template <IsComponent T>
T ComponentData::Unwrap() const {
  return std::move(*static_cast<T *>(raw_ptr_));
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_COMPONENT_PACKAGE
