#include <gtest/gtest.h>

#include "mirage_ecs/entity/archetype.hpp"
#include "mirage_ecs/util/type_id.hpp"

using namespace mirage;
using namespace mirage::ecs;

struct Bool : Component {
  bool value{false};
};

struct Int32 : Component {
  int32_t value{0};
};

struct Int64 : Component {
  int64_t value{0};
};

TEST(ArchetypeTests, OffsetAndAlignment) {
  const auto archetype = Archetype::New<Bool, Int64, Int32>();
  EXPECT_EQ(archetype.data_chunk_header()->entity_align, 8);
  EXPECT_EQ(archetype.data_chunk_header()->entity_size, 16);

  const auto& offset_map = archetype.data_chunk_header()->type_addr_offset_map;
  EXPECT_EQ(offset_map.TryFind(TypeId::Of<Int64>())->val(), 0);
  EXPECT_EQ(offset_map.TryFind(TypeId::Of<Int32>())->val(), 8);
  EXPECT_EQ(offset_map.TryFind(TypeId::Of<Bool>())->val(), 12);
}
