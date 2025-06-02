#include "mirage_ecs/component/component_bundle.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

Optional<Box<Component>> ComponentBundle::Add(Box<Component> component) {
  MIRAGE_DCHECK(component.is_valid());

  auto kv_opt =
      component_map_.Insert(component.type_id(), std::move(component));
  if (!kv_opt.is_valid()) {
    return Optional<Box<Component>>::None();
  }
  auto kv = kv_opt.Unwrap();
  return Optional<Box<Component>>::New(std::move(kv.val()));
}

Optional<Box<Component>> ComponentBundle::Remove(const TypeId &type_id) {
  auto kv_opt = component_map_.Remove(type_id);
  if (!kv_opt.is_valid()) {
    return Optional<Box<Component>>::None();
  }
  auto kv = kv_opt.Unwrap();
  return Optional<Box<Component>>::New(std::move(kv.val()));
}

TypeSet ComponentBundle::MakeTypeSet() const {
  TypeSet type_set;
  type_set.Reserve(component_map_.size());
  for (const auto &kv : component_map_) {
    type_set.AddTypeId(kv.key());
  }
  return type_set;
}

const ComponentBundle::ComponentMap &ComponentBundle::component_map() const {
  return component_map_;
}

size_t ComponentBundle::size() const { return component_map_.size(); }
