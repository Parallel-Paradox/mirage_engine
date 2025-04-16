#include <gtest/gtest.h>

#include "mirage_ecs/entity/archetype.hpp"
#include "mirage_ecs/util/type_id.hpp"

using namespace mirage;
using namespace mirage::ecs;

TEST(ArchetypeTests, OffsetAndAlignment) {
  const auto archetype = Archetype::New<int64_t, int32_t, bool>();
  EXPECT_EQ(archetype.GetEntityAlign(), 8);
  EXPECT_EQ(archetype.GetEntitySize(), 16);

  const auto& offset_map = archetype.GetTypeAddrOffsetMap();
  EXPECT_EQ(offset_map.TryFind(TypeId::Of<int64_t>())->val, 0);
  EXPECT_EQ(offset_map.TryFind(TypeId::Of<int32_t>())->val, 8);
  EXPECT_EQ(offset_map.TryFind(TypeId::Of<bool>())->val, 12);
}
