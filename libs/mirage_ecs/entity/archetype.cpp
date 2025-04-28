#include "mirage_ecs/entity/archetype.hpp"

#include <algorithm>
#include <numeric>

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_ecs/entity/data_chunk.hpp"
#include "mirage_ecs/util/type_set.hpp"

using namespace mirage;
using namespace mirage::ecs;

Archetype::Archetype(TypeSet&& type_set)
    : type_set_(std::move(type_set)),
      data_chunk_header_(new DataChunk::Header()) {
  if (type_set_.GetTypeArray().empty()) {
    return;
  }

  size_t entity_align = type_set_.GetTypeArray()[0].type_align();
  for (const TypeId& type_id : type_set_.GetTypeArray()) {
    entity_align = std::lcm(entity_align, type_id.type_align());
  }
  data_chunk_header_->entity_align = entity_align;

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
    data_chunk_header_->type_addr_offset_map.Insert(type_id, offset);
    offset += type_id.type_size();
  }

  if (offset % entity_align != 0) {
    offset += entity_align - (offset % entity_align);
  }
  data_chunk_header_->entity_size = offset;
}

const base::SharedLocal<DataChunk::Header>& Archetype::data_chunk_header()
    const {
  return data_chunk_header_;
}

const TypeSet& Archetype::type_set() const { return type_set_; }
