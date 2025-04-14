#include "mirage_ecs/archetype/archetype.hpp"

#include <algorithm>
#include <numeric>

using namespace mirage;
using namespace mirage::ecs;

Archetype::Archetype(Descriptor&& descriptor)
    : descriptor_(std::move(descriptor)) {
  if (descriptor_.GetTypeArray().IsEmpty()) {
    return;
  }

  entity_align_ = descriptor_.GetTypeArray()[0].GetTypeAlign();
  for (const TypeId& type_id : descriptor_.GetTypeArray()) {
    entity_align_ = std::lcm(entity_align_, type_id.GetTypeAlign());
  }

  base::Array<TypeId> type_array = descriptor_.GetTypeArray();
  auto cmp = [](const TypeId& lhs, const TypeId& rhs) {
    if (lhs.GetTypeAlign() == rhs.GetTypeAlign()) {
      return lhs.GetTypeSize() > rhs.GetTypeSize();
    }
    return lhs.GetTypeAlign() > rhs.GetTypeAlign();
  };
  std::ranges::sort(type_array, cmp);

  size_t offset = 0;
  for (const TypeId& type_id : type_array) {
    const size_t type_align = type_id.GetTypeAlign();
    if (offset % type_align != 0) {
      offset += type_align - (offset % type_align);
    }
    type_addr_offset_map_.Insert(type_id, offset);
    offset += type_id.GetTypeSize();
  }
}

const Archetype::Descriptor& Archetype::GetDescriptor() const {
  return descriptor_;
}

size_t Archetype::GetChunkAlign() const { return entity_align_; }

const base::HashMap<TypeId, size_t>& Archetype::GetTypeAddrOffsetMap() const {
  return type_addr_offset_map_;
}
