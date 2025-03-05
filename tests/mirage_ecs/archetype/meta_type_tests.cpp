#include <gtest/gtest.h>

#include "mirage_ecs/archetype/meta_type.hpp"

using namespace mirage::ecs;

TEST(MetaTypeTests, Consistent) {
  const MetaType *meta_type = MetaType::Of<size_t>();
  EXPECT_EQ(meta_type, MetaType::Of<size_t>());
}
