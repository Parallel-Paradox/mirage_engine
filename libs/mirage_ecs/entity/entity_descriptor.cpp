#include "mirage_ecs/entity/entity_descriptor.hpp"

#include <numeric>

#include "mirage_base/container/array.hpp"
#include "mirage_ecs/component/component_id.hpp"
#include "mirage_ecs/util/type_set.hpp"

using namespace mirage::ecs;

EntityDescriptor::EntityDescriptor(
    base::Array<ComponentId> component_id_array) {
  // Build type set and remove duplicates.
  type_set_.Reserve(component_id_array.size());
  for (auto iter = component_id_array.begin();
       iter != component_id_array.end();) {
    const size_t old_size = type_set_.size();
    type_set_.AddTypeId(iter->type_id());
    if (type_set_.size() == old_size) {
      component_id_array.SwapRemove(iter - component_id_array.begin());
    } else {
      ++iter;
    }
  }
  type_set_.ShrinkToFit();

  // Set the least common multiple of all component alignments as the entity
  // alignment.
  align_ = type_set_.type_array()[0].type_align();
  for (const TypeId& type_id : type_set_.type_array()) {
    align_ = std::lcm(align_, type_id.type_align());
  }

  // Layout components in descending order of alignment and size. Set offsets.
  auto cmp = [](const ComponentId& lhs, const ComponentId& rhs) {
    if (lhs.type_id().type_align() == rhs.type_id().type_align()) {
      return lhs.type_id().type_size() > rhs.type_id().type_size();
    }
    return lhs.type_id().type_align() > rhs.type_id().type_align();
  };
  std::ranges::sort(component_id_array, cmp);

  size_t offset = 0;
  for (const ComponentId& component_id : component_id_array) {
    auto type_id = component_id.type_id();
    if (const size_t type_align = type_id.type_align();
        offset % type_align != 0) {
      offset += type_align - (offset % type_align);
    }
    offset_map_[component_id] = offset;
    offset += type_id.type_size();
  }

  // Align the end of the entity.
  if (offset % align_ != 0) {
    offset += align_ - (offset % align_);
  }
  size_ = offset;
}

size_t EntityDescriptor::align() const { return align_; }

size_t EntityDescriptor::size() const { return size_; }

const EntityDescriptor::OffsetMap& EntityDescriptor::offset_map() const {
  return offset_map_;
}

const TypeSet& EntityDescriptor::type_set() const { return type_set_; }
