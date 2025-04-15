#include <gtest/gtest.h>

#include "mirage_ecs/util/type_id.hpp"

using namespace mirage::ecs;

TEST(TypeIdTests, Consistent) {
  const TypeId type_id = TypeId::Of<size_t>();
  EXPECT_EQ(type_id, TypeId::Of<size_t>());
  EXPECT_NE(type_id, TypeId::Of<int32_t>());
}

TEST(TypeIdTests, Information) {
  const TypeId type_id = TypeId::Of<size_t>();
  EXPECT_STREQ(type_id.GetTypeName(), typeid(size_t).name());
  EXPECT_EQ(type_id.GetTypeSize(), sizeof(size_t));
  EXPECT_EQ(type_id.GetTypeAlign(), alignof(size_t));
  EXPECT_EQ(type_id.GetHashCode(), typeid(size_t).hash_code());
  EXPECT_EQ(type_id.GetBitFlag(), static_cast<size_t>(1)
                                      << typeid(size_t).hash_code() % 64);
}
