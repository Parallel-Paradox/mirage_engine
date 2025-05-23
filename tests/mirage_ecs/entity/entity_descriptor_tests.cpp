#include <gtest/gtest.h>

#include "mirage_ecs/entity/entity_descriptor.hpp"
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

TEST(EntityDescriptorTests, OffsetAndAlignment) {
  const auto layout = EntityDescriptor::New<Bool, Int64, Int32>();
  EXPECT_EQ(layout.align(), 8);
  EXPECT_EQ(layout.size(), 16);

  const auto& type_set = layout.component_type_set();
  EXPECT_TRUE(type_set.With(TypeSet::New<Bool, Int64, Int32>()));

  const auto& offset_map = layout.component_meta_map();
  EXPECT_EQ(offset_map[base::TypeId::Of<Int64>()].offset, 0);
  EXPECT_EQ(offset_map[base::TypeId::Of<Int32>()].offset, 8);
  EXPECT_EQ(offset_map[base::TypeId::Of<Bool>()].offset, 12);
}
