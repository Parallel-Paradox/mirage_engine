#include "mirage_ecs/component/component_id.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

bool ComponentMeta::operator==(const ComponentMeta &other) const {
  return type_id_ == other.type_id_;
}

bool ComponentMeta::operator!=(const ComponentMeta &other) const {
  return type_id_ != other.type_id_;
}

std::strong_ordering ComponentMeta::operator<=>(
    const ComponentMeta &other) const {
  return type_id_ <=> other.type_id_;
}

TypeId ComponentMeta::type_id() const { return type_id_; }

ComponentMeta::DestructFunc ComponentMeta::destruct_func() const {
  return destruct_func_;
}

ComponentMeta::MoveFunc ComponentMeta::move_func() const { return move_func_; }

ComponentMeta::ComponentMeta(TypeId type_id, DestructFunc destruct_func,
                             MoveFunc move_func)
    : type_id_(type_id), destruct_func_(destruct_func), move_func_(move_func) {}

ComponentId::ComponentId(const ComponentMeta &component_meta)
    : component_meta_(&component_meta) {}

bool ComponentId::operator==(const ComponentId &other) const {
  return *component_meta_ == *other.component_meta_;
}

bool ComponentId::operator!=(const ComponentId &other) const {
  return *component_meta_ != *other.component_meta_;
}

std::strong_ordering ComponentId::operator<=>(const ComponentId &other) const {
  return *component_meta_ <=> *other.component_meta_;
}

bool ComponentId::operator==(const TypeId &other) const {
  return component_meta_->type_id() == other;
}

TypeId ComponentId::type_id() const { return component_meta_->type_id(); }

ComponentMeta::DestructFunc ComponentId::destruct_func() const {
  return component_meta_->destruct_func();
}

ComponentMeta::MoveFunc ComponentId::move_func() const {
  return component_meta_->move_func();
}
