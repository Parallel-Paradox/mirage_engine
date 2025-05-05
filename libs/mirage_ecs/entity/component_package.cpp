#include "mirage_ecs/entity/component_package.hpp"

using namespace mirage::ecs;

ComponentData::~ComponentData() {
  if (raw_ptr_ == nullptr) {
    return;
  }
  destroy_func_(raw_ptr_);
  raw_ptr_ = nullptr;
  destroy_func_ = nullptr;
}

ComponentData::ComponentData(ComponentData &&other) noexcept
    : raw_ptr_(other.raw_ptr_),
      type_id_(other.type_id_),
      destroy_func_(other.destroy_func_) {
  other.raw_ptr_ = nullptr;
  other.destroy_func_ = nullptr;
}

ComponentData &ComponentData::operator=(ComponentData &&other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~ComponentData();
  new (this) ComponentData(std::move(other));
  return *this;
}

void *ComponentData::raw_ptr() const { return raw_ptr_; }

const TypeId &ComponentData::type_id() const { return type_id_; }
