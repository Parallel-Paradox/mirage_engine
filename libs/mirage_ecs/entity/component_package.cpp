#include "mirage_ecs/entity/component_package.hpp"

#include <utility>

#include "mirage_base/define/check.hpp"
#include "mirage_base/wrap/optional.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

Optional<Box<Component>> ComponentPackage::Remove(const TypeId &type_id) {
  type_set_.RemoveTypeId(type_id);
  auto kv_optional = component_data_map_.Remove(type_id);
  if (!kv_optional.is_valid()) {
    return Optional<Box<Component>>::None();
  }
  auto kv = kv_optional.Unwrap();
  return Optional<Box<Component>>::New(std::move(kv.val()));
}

size_t ComponentPackage::size() const { return component_data_map_.size(); }

const TypeSet &ComponentPackage::type_set() const { return type_set_; }

const ComponentPackage::ComponentMap &ComponentPackage::component_data_map()
    const {
  return component_data_map_;
}

Optional<Box<Component>> ComponentPackage::Add(Box<Component> component) {
  MIRAGE_DCHECK(component.is_valid());

  type_set_.AddTypeId(component.type_id());
  auto kv_optional =
      component_data_map_.Insert(component.type_id(), std::move(component));
  if (!kv_optional.is_valid()) {
    return Optional<Box<Component>>::None();
  }
  auto kv = kv_optional.Unwrap();
  return Optional<Box<Component>>::New(std::move(kv.val()));
}
