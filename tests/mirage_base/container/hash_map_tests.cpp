#include <gtest/gtest.h>

#include "mirage_base/container/hash_map.hpp"

using namespace mirage::base;

TEST(HashMapTests, Construct) {
  const HashMap<int32_t, int32_t> map{{1, 1}, {2, 2}};
  EXPECT_FALSE(map.IsEmpty());
  EXPECT_EQ(map.GetSize(), 2);
  EXPECT_EQ(*map.TryFind(1), 1);
  EXPECT_EQ(*map.TryFind(2), 2);
}
