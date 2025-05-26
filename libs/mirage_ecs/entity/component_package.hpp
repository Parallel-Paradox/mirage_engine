#ifndef MIRAGE_ECS_ENTITY_COMPONENT_PACKAGE
#define MIRAGE_ECS_ENTITY_COMPONENT_PACKAGE

#include <cstddef>

#include "mirage_base/container/hash_map.hpp"
#include "mirage_base/util/type_id.hpp"
#include "mirage_base/wrap/box.hpp"
#include "mirage_base/wrap/optional.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/util/marker.hpp"
#include "mirage_ecs/util/type_set.hpp"

namespace mirage::ecs {

class ComponentPackage {
  using TypeId = base::TypeId;
  using Box = base::Box;

  template <typename T>
  using Optional = base::Optional<T>;

 public:
  using ComponentMap = base::HashMap<TypeId, Box>;

  MIRAGE_ECS ComponentPackage() = default;
  MIRAGE_ECS ~ComponentPackage() = default;

  ComponentPackage(const ComponentPackage &) = delete;
  ComponentPackage &operator=(const ComponentPackage &) = delete;

  MIRAGE_ECS ComponentPackage(ComponentPackage &&) = default;
  MIRAGE_ECS ComponentPackage &operator=(ComponentPackage &&) = default;

  template <IsComponent T>
  Optional<T> Add(T components);

  template <IsComponent T>
  Optional<T> Remove();
  MIRAGE_ECS Optional<Box> Remove(const TypeId &type_id);

  [[nodiscard]] MIRAGE_ECS size_t size() const;
  [[nodiscard]] MIRAGE_ECS const TypeSet &type_set() const;
  [[nodiscard]] MIRAGE_ECS const ComponentMap &component_data_map() const;

 private:
  MIRAGE_ECS Optional<Box> Add(Box component);

  TypeSet type_set_;
  ComponentMap component_data_map_;
};

template <IsComponent T>
base::Optional<T> ComponentPackage::Add(T component) {
  Optional<Box> component_optional = Add(Box::New<T>(std::move(component)));
  if (!component_optional.is_valid()) {
    return Optional<T>::None();
  }
  return Optional<T>::New(component_optional.Unwrap().Unwrap<T>());
}

template <IsComponent T>
base::Optional<T> ComponentPackage::Remove() {
  Optional<Box> component_optional = Remove(TypeId::Of<T>());
  if (!component_optional.is_valid()) {
    return Optional<T>::None();
  }
  return Optional<T>::New(component_optional.Unwrap().Unwrap<T>());
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_COMPONENT_PACKAGE
