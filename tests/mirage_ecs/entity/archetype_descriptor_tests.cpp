#include <gtest/gtest.h>

#include "mirage_ecs/entity/archetype_descriptor.hpp"
#include "mirage_ecs/util/type_set.hpp"

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

TEST(ArchetypeDescriptorTests, LayoutCheck) {
  const auto desc = ArchetypeDescriptor::New<Bool, Int64, Int32>();
  EXPECT_EQ(desc.align(), 8);
  EXPECT_EQ(desc.size(), 16);
  EXPECT_EQ(desc.type_set(), (TypeSet::New<Bool, Int64, Int32>()));

  const auto& offset_map = desc.offset_map();
  EXPECT_EQ(offset_map[ComponentId::Of<Int64>()], 0);
  EXPECT_EQ(offset_map[ComponentId::Of<Int32>()], 8);
  EXPECT_EQ(offset_map[ComponentId::Of<Bool>()], 12);
}

TEST(ArchetypeDescriptorTests, DuplicateInit) {
  const auto desc = ArchetypeDescriptor::New<Bool, Int64, Int32, Int64>();
  EXPECT_EQ(desc.align(), 8);
  EXPECT_EQ(desc.size(), 16);
  EXPECT_EQ(desc.type_set(), (TypeSet::New<Bool, Int64, Int32>()));

  const auto& offset_map = desc.offset_map();
  EXPECT_EQ(offset_map[ComponentId::Of<Int64>()], 0);
  EXPECT_EQ(offset_map[ComponentId::Of<Int32>()], 8);
  EXPECT_EQ(offset_map[ComponentId::Of<Bool>()], 12);
}
