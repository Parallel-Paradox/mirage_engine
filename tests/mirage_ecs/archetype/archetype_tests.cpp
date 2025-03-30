#include <gtest/gtest.h>

#include "mirage_ecs/archetype/archetype.hpp"

using namespace mirage;
using namespace mirage::ecs;

TEST(ArchetypeTests, Descriptor) {
  EXPECT_TRUE(base::HashType<Archetype::Descriptor>);
  auto desc = Archetype::Descriptor::Build<size_t>();

  desc.AddType<int32_t>();
  const auto expect_desc =
      Archetype::Descriptor::Build<size_t, int32_t, size_t>();
  EXPECT_EQ(desc, expect_desc);
  EXPECT_EQ(desc.GetTypeArray(), expect_desc.GetTypeArray());

  desc.AddType<int32_t>();
  EXPECT_EQ(desc, expect_desc);
}
