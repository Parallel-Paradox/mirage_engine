#include "mirage_ecs/entity/entity_descriptor.hpp"

#include <algorithm>
#include <cstddef>
#include <numeric>

#include "mirage_ecs/util/type_set.hpp"

using namespace mirage::ecs;

size_t EntityDescriptor::align() const { return align_; }

size_t EntityDescriptor::size() const { return size_; }

const TypeSet& EntityDescriptor::component_type_set() const {
  return component_type_set_;
}

const EntityDescriptor::ComponentMetaMap& EntityDescriptor::component_meta_map()
    const {
  return component_meta_map_;
}

void EntityDescriptor::InitializeLayout(TypeSet&& type_set) {
  size_t entity_align = type_set.type_array()[0].type_align();
  for (const TypeId& type_id : type_set.type_array()) {
    entity_align = std::lcm(entity_align, type_id.type_align());
  }
  align_ = entity_align;

  base::Array<TypeId> type_array = type_set.type_array();
  auto cmp = [](const TypeId& lhs, const TypeId& rhs) {
    if (lhs.type_align() == rhs.type_align()) {
      return lhs.type_size() > rhs.type_size();
    }
    return lhs.type_align() > rhs.type_align();
  };
  std::ranges::sort(type_array, cmp);

  size_t offset = 0;
  for (const TypeId& type_id : type_array) {
    if (const size_t type_align = type_id.type_align();
        offset % type_align != 0) {
      offset += type_align - (offset % type_align);
    }
    component_meta_map_[type_id].offset = offset;
    offset += type_id.type_size();
  }

  if (offset % entity_align != 0) {
    offset += entity_align - (offset % entity_align);
  }
  size_ = offset;

  component_type_set_ = std::move(type_set);
}
