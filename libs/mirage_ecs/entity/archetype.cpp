#include "mirage_ecs/entity/archetype.hpp"

#include <algorithm>
#include <numeric>

#include "mirage_ecs/util/type_set.hpp"

using namespace mirage;
using namespace mirage::ecs;

Archetype::Archetype(TypeSet&& type_set) : type_set_(std::move(type_set)) {
  if (type_set_.GetTypeArray().empty()) {
    return;
  }

  entity_align_ = type_set_.GetTypeArray()[0].type_align();
  for (const TypeId& type_id : type_set_.GetTypeArray()) {
    entity_align_ = std::lcm(entity_align_, type_id.type_align());
  }

  base::Array<TypeId> type_array = type_set_.GetTypeArray();
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
    type_addr_offset_map_.Insert(type_id, offset);
    offset += type_id.type_size();
  }

  if (offset % entity_align_ != 0) {
    offset += entity_align_ - (offset % entity_align_);
  }
  entity_size_ = offset;
}

const TypeSet& Archetype::type_set() const { return type_set_; }

size_t Archetype::entity_align() const { return entity_align_; }

size_t Archetype::entity_size() const { return entity_size_; }

const base::HashMap<TypeId, size_t>& Archetype::type_addr_offset_map() const {
  return type_addr_offset_map_;
}
