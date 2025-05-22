#ifndef MIRAGE_ECS_ENTITY_COMPONENT_PACKAGE
#define MIRAGE_ECS_ENTITY_COMPONENT_PACKAGE

#include <cstddef>

#include "mirage_base/container/hash_map.hpp"
#include "mirage_base/util/type_id.hpp"
#include "mirage_base/wrap/optional.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/component_func_table.hpp"
#include "mirage_ecs/util/marker.hpp"
#include "mirage_ecs/util/type_set.hpp"

namespace mirage::ecs {

class ComponentData {
 public:
  using TypeId = base::TypeId;

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

  MIRAGE_ECS void *Take() &&;

  [[nodiscard]] MIRAGE_ECS void *raw_ptr() const;
  [[nodiscard]] MIRAGE_ECS const TypeId &type_id() const;

 private:
  MIRAGE_ECS ComponentData(void *raw_ptr, TypeId type_id,
                           void (*destroy_func)(void *));

  void *raw_ptr_{nullptr};

  TypeId type_id_;
  void (*destroy_func_)(void *){nullptr};
};

class ComponentPackage {
 public:
  using TypeId = base::TypeId;
  using ComponentDataMap = base::HashMap<TypeId, ComponentData>;

  MIRAGE_ECS ComponentPackage() = default;
  MIRAGE_ECS ~ComponentPackage() = default;

  ComponentPackage(const ComponentPackage &) = delete;
  ComponentPackage &operator=(const ComponentPackage &) = delete;

  MIRAGE_ECS ComponentPackage(ComponentPackage &&) = default;
  MIRAGE_ECS ComponentPackage &operator=(ComponentPackage &&) = default;

  template <IsComponent T>
  base::Optional<T> Add(T components);
  MIRAGE_ECS base::Optional<ComponentData> Add(ComponentData component_data);

  template <IsComponent T>
  base::Optional<T> Remove();
  MIRAGE_ECS base::Optional<ComponentData> Remove(const TypeId &type_id);

  [[nodiscard]] MIRAGE_ECS size_t size() const;
  [[nodiscard]] MIRAGE_ECS const TypeSet &type_set() const;
  [[nodiscard]] MIRAGE_ECS const ComponentDataMap &component_data_map() const;

 private:
  TypeSet type_set_;
  ComponentDataMap component_data_map_;
};

template <IsComponent T>
ComponentData ComponentData::New(T component) {
  return ComponentData(static_cast<void *>(new T(std::move(component))),
                       TypeId::Of<T>(),
                       [](void *raw_ptr) { delete static_cast<T *>(raw_ptr); });
}

template <IsComponent T>
T ComponentData::Unwrap() const {
  MIRAGE_DCHECK(type_id_ == TypeId::Of<T>());
  return std::move(*static_cast<T *>(raw_ptr_));
}

template <IsComponent T>
base::Optional<T> ComponentPackage::Add(T component) {
  base::Optional<ComponentData> component_data_optional =
      Add(ComponentData::New<T>(std::move(component)));
  if (!component_data_optional.is_valid()) {
    return base::Optional<T>::None();
  }
  auto component_data = component_data_optional.Unwrap();
  return base::Optional<T>::New(component_data.template Unwrap<T>());
}

template <IsComponent T>
base::Optional<T> ComponentPackage::Remove() {
  base::Optional<ComponentData> component_data_optional =
      Remove(TypeId::Of<T>());
  if (!component_data_optional.is_valid()) {
    return base::Optional<T>::None();
  }
  auto component_data = component_data_optional.Unwrap();
  return base::Optional<T>::New(component_data.template Unwrap<T>());
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_COMPONENT_PACKAGE
