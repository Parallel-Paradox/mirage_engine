#include <gtest/gtest.h>

#include "mirage_framework/base/util/meta_type.hpp"
#include "mirage_framework/base/util/optional.hpp"

using namespace mirage;

TEST(UtilTests, MetaType) {
  const MetaType* type_info = MetaType::Of<int32_t>();
  EXPECT_STREQ(type_info->GetName(), typeid(int32_t).name());
  EXPECT_EQ(type_info->GetSize(), sizeof(int32_t));
  EXPECT_EQ(type_info, MetaType::Of<int32_t>());
  EXPECT_NE(type_info, MetaType::Of<int64_t>());
}

TEST(UtilTests, NonOptional) {
  auto num = Optional<int32_t>::None();
  EXPECT_FALSE(num.IsValid());
}

TEST(UtilTests, UnwrapOptional) {
  auto num = Optional<int32_t>::New(1);
  EXPECT_TRUE(num.IsValid());
  EXPECT_EQ(num.Unwrap(), 1);
  EXPECT_FALSE(num.IsValid());
}

TEST(UtilTests, MoveOptional) {
  auto num = Optional<int32_t>::New(1);
  EXPECT_TRUE(num.IsValid());
  auto move_num = std::move(num);
  EXPECT_FALSE(num.IsValid());
  EXPECT_TRUE(move_num.IsValid());
  EXPECT_EQ(move_num.Unwrap(), 1);
}
