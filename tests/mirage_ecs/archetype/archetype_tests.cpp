#include <gtest/gtest.h>

#include "mirage_ecs/archetype/archetype.hpp"

using namespace mirage;
using namespace mirage::ecs;

TEST(ArchetypeTests, Descriptor) {
  EXPECT_TRUE(base::HashType<ArchetypeDescriptor>);
  auto desc = ArchetypeDescriptor::New<size_t>();

  desc.AddType<int32_t>();
  const auto expect_desc = ArchetypeDescriptor::New<size_t, int32_t, size_t>();
  EXPECT_EQ(desc, expect_desc);
  EXPECT_EQ(desc.GetTypeArray(), expect_desc.GetTypeArray());

  desc.AddType<int32_t>();
  EXPECT_EQ(desc, expect_desc);
}

TEST(ArchetypeTests, QueryMatch) {
  const auto desc = ArchetypeDescriptor::New<size_t, int32_t>();
  EXPECT_TRUE(desc.With(desc));
  EXPECT_FALSE(desc.Without(desc));

  const auto empty_desc = ArchetypeDescriptor();
  EXPECT_TRUE(desc.With(empty_desc));
  EXPECT_FALSE(empty_desc.With(desc));
  EXPECT_TRUE(desc.Without(empty_desc));
  EXPECT_TRUE(empty_desc.Without(desc));

  const auto with_desc_success = ArchetypeDescriptor::New<size_t>();
  const auto without_desc_success = ArchetypeDescriptor::New<bool>();
  EXPECT_TRUE(desc.With(with_desc_success));
  EXPECT_TRUE(desc.Without(without_desc_success));
  EXPECT_FALSE(desc.With(without_desc_success));
  EXPECT_FALSE(desc.Without(with_desc_success));

  const auto with_desc_fail_0 =
      ArchetypeDescriptor::New<size_t, int32_t, bool>();
  const auto with_desc_fail_1 = ArchetypeDescriptor::New<int32_t, bool>();
  EXPECT_FALSE(desc.With(with_desc_fail_0));
  EXPECT_FALSE(desc.With(with_desc_fail_1));
  EXPECT_FALSE(desc.Without(with_desc_fail_0));
  EXPECT_FALSE(desc.Without(with_desc_fail_1));
}
