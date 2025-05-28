#ifndef MIRAGE_ECS_COMPONENT_COMPONENT_BUNDLE
#define MIRAGE_ECS_COMPONENT_COMPONENT_BUNDLE

#include "mirage_base/container/hash_map.hpp"
#include "mirage_base/util/type_id.hpp"
#include "mirage_base/wrap/box.hpp"
#include "mirage_ecs/util/marker.hpp"
#include "mirage_ecs/util/type_set.hpp"

namespace mirage::ecs {

class ComponentBundle {
  using TypeId = base::TypeId;

  template <typename T>
  using Box = base::Box<T>;
  template <typename T>
  using Optional = base::Optional<T>;

 public:
  using ComponentMap = base::HashMap<TypeId, Box<Component>>;

  MIRAGE_ECS ComponentBundle() = default;
  MIRAGE_ECS ~ComponentBundle() = default;

  MIRAGE_ECS ComponentBundle(const ComponentBundle &) = delete;
  MIRAGE_ECS ComponentBundle &operator=(const ComponentBundle &) = delete;

  MIRAGE_ECS ComponentBundle(ComponentBundle &&) = default;
  MIRAGE_ECS ComponentBundle &operator=(ComponentBundle &&) = default;

  template <IsComponent T>
  Optional<T> Add(T components);
  MIRAGE_ECS Optional<Box<Component>> Add(Box<Component> component);

  template <IsComponent T>
  Optional<T> Remove();
  MIRAGE_ECS Optional<Box<Component>> Remove(const TypeId &type_id);

  [[nodiscard]] MIRAGE_ECS TypeSet MakeTypeSet() const;

  [[nodiscard]] MIRAGE_ECS const ComponentMap &component_map() const;
  [[nodiscard]] MIRAGE_ECS size_t size() const;

 private:
  ComponentMap component_map_;
};

template <IsComponent T>
base::Optional<T> ComponentBundle::Add(T component) {
  Optional<Box<Component>> old_component_opt =
      Add(Box<Component>(std::move(component)));
  if (!old_component_opt.is_valid()) {
    return Optional<T>::None();
  }
  return Optional<T>::New(old_component_opt.Unwrap().Unwrap<T>());
}

template <IsComponent T>
base::Optional<T> ComponentBundle::Remove() {
  Optional<Box<Component>> component_opt = Remove(TypeId::Of<T>());
  if (!component_opt.is_valid()) {
    return Optional<T>::None();
  }
  return Optional<T>::New(component_opt.Unwrap().Unwrap<T>());
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_COMPONENT_COMPONENT_BUNDLE
