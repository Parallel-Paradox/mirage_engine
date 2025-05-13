#include "mirage_ecs/entity/component_package.hpp"

#include <utility>

#include "mirage_base/util/optional.hpp"

using namespace mirage;
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

ComponentData::ComponentData(void *raw_ptr, TypeId type_id,
                             void (*destroy_func)(void *))
    : raw_ptr_(raw_ptr), type_id_(type_id), destroy_func_(destroy_func) {}

base::Optional<ComponentData> ComponentPackage::Add(
    ComponentData component_data) {
  type_set_.AddTypeId(component_data.type_id());
  auto kv_optional = component_data_map_.Insert(component_data.type_id(),
                                                std::move(component_data));
  if (!kv_optional.is_valid()) {
    return base::Optional<ComponentData>::None();
  }
  auto kv = kv_optional.Unwrap();
  return base::Optional<ComponentData>::New(std::move(kv.val()));
}

base::Optional<ComponentData> ComponentPackage::Remove(const TypeId &type_id) {
  type_set_.RemoveTypeId(type_id);
  auto kv_optional = component_data_map_.Remove(type_id);
  if (!kv_optional.is_valid()) {
    return base::Optional<ComponentData>::None();
  }
  auto kv = kv_optional.Unwrap();
  return base::Optional<ComponentData>::New(std::move(kv.val()));
}

size_t ComponentPackage::size() const { return component_data_map_.size(); }

const TypeSet &ComponentPackage::type_set() const { return type_set_; }

const ComponentPackage::ComponentDataMap &ComponentPackage::component_data_map()
    const {
  return component_data_map_;
}
