#include <gtest/gtest.h>

#include "mirage_ecs/entity/archetype.hpp"
#include "mirage_ecs/util/type_id.hpp"

using namespace mirage;
using namespace mirage::ecs;

TEST(ArchetypeTests, OffsetAndAlignment) {
  const auto archetype = Archetype::New<bool, int64_t, int32_t>();
  EXPECT_EQ(archetype.entity_align(), 8);
  EXPECT_EQ(archetype.entity_size(), 16);

  const auto& offset_map = archetype.type_addr_offset_map();
  EXPECT_EQ(offset_map.TryFind(TypeId::Of<int64_t>())->val(), 0);
  EXPECT_EQ(offset_map.TryFind(TypeId::Of<int32_t>())->val(), 8);
  EXPECT_EQ(offset_map.TryFind(TypeId::Of<bool>())->val(), 12);
}
