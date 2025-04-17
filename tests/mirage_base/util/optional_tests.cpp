#include <gtest/gtest.h>

#include "mirage_base/util/optional.hpp"

using namespace mirage::base;

TEST(UtilTests, UnwrapOptional) {
  auto num = Optional<int32_t>::None();
  EXPECT_FALSE(num.is_valid());
  // num.Unwrap(); // Panic if try to unwrap invalid Optional.

  num = Optional<int32_t>::New(1);
  EXPECT_TRUE(num.is_valid());
  EXPECT_EQ(num.Unwrap(), 1);
  EXPECT_FALSE(num.is_valid());
}

TEST(UtilTests, MoveOptional) {
  auto num = Optional<int32_t>::New(1);
  EXPECT_TRUE(num.is_valid());
  Optional<int32_t> move_num(std::move(num));
  EXPECT_FALSE(num.is_valid());  // NOLINT(*-use-after-move): Allow for test.
  EXPECT_TRUE(move_num.is_valid());
  EXPECT_EQ(move_num.Unwrap(), 1);
}
