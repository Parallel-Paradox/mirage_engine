#include <gtest/gtest.h>

#include "mirage_ecs/archetype/type_meta.hpp"

using namespace mirage::ecs;

TEST(TypeMetaTests, Consistent) {
  const TypeMeta *meta_type = TypeMeta::Of<size_t>();
  EXPECT_EQ(meta_type, TypeMeta::Of<size_t>());
  EXPECT_NE(meta_type, TypeMeta::Of<int32_t>());
}

TEST(TypeMetaTests, GetId) {
  const TypeMeta *meta_type = TypeMeta::Of<size_t>();
  EXPECT_EQ(reinterpret_cast<size_t>(meta_type), meta_type->GetTypeId());
}
