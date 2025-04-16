#include "mirage_ecs/entity/archetype.hpp"

#include <algorithm>
#include <numeric>

#include "mirage_ecs/util/type_set.hpp"

using namespace mirage;
using namespace mirage::ecs;

Archetype::Archetype(TypeSet&& type_set) : type_set_(std::move(type_set)) {
  if (type_set_.GetTypeArray().IsEmpty()) {
    return;
  }

  entity_align_ = type_set_.GetTypeArray()[0].GetTypeAlign();
  for (const TypeId& type_id : type_set_.GetTypeArray()) {
    entity_align_ = std::lcm(entity_align_, type_id.GetTypeAlign());
  }

  base::Array<TypeId> type_array = type_set_.GetTypeArray();
  auto cmp = [](const TypeId& lhs, const TypeId& rhs) {
    if (lhs.GetTypeAlign() == rhs.GetTypeAlign()) {
      return lhs.GetTypeSize() > rhs.GetTypeSize();
    }
    return lhs.GetTypeAlign() > rhs.GetTypeAlign();
  };
  std::ranges::sort(type_array, cmp);

  size_t offset = 0;
  for (const TypeId& type_id : type_array) {
    if (const size_t type_align = type_id.GetTypeAlign();
        offset % type_align != 0) {
      offset += type_align - (offset % type_align);
    }
    type_addr_offset_map_.Insert(type_id, offset);
    offset += type_id.GetTypeSize();
  }

  if (offset % entity_align_ != 0) {
    offset += entity_align_ - (offset % entity_align_);
  }
  entity_size_ = offset;
}

const TypeSet& Archetype::GetTypeSet() const { return type_set_; }

size_t Archetype::GetEntityAlign() const { return entity_align_; }

size_t Archetype::GetEntitySize() const { return entity_size_; }

const base::HashMap<TypeId, size_t>& Archetype::GetTypeAddrOffsetMap() const {
  return type_addr_offset_map_;
}
