#include <gtest/gtest.h>

#include "mirage_base/util/type_id.hpp"

using namespace mirage::base;

TEST(TypeIdTests, Consistent) {
  const TypeId type_id = TypeId::Of<size_t>();
  EXPECT_EQ(type_id, TypeId::Of<size_t>());
  EXPECT_NE(type_id, TypeId::Of<int32_t>());
}

TEST(TypeIdTests, Information) {
  const TypeId type_id = TypeId::Of<size_t>();
  EXPECT_EQ(type_id.type_index(), typeid(size_t));
  EXPECT_STREQ(type_id.type_name(), typeid(size_t).name());
  EXPECT_EQ(type_id.type_size(), sizeof(size_t));
  EXPECT_EQ(type_id.type_align(), alignof(size_t));
  EXPECT_EQ(type_id.hash_code(), typeid(size_t).hash_code());
  EXPECT_EQ(type_id.bit_flag(), static_cast<size_t>(1)
                                    << typeid(size_t).hash_code() % 64);
}
