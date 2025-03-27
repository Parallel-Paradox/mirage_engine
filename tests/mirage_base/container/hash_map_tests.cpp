#include <gtest/gtest.h>

#include "mirage_base/container/hash_map.hpp"

using namespace mirage::base;

TEST(HashMapTests, Construct) {
  const HashMap<int32_t, int32_t> map{{1, 1}, {2, 2}};
  EXPECT_FALSE(map.IsEmpty());
  EXPECT_EQ(map.GetSize(), 2);
  EXPECT_EQ(map.TryFind(1)->val, 1);
  EXPECT_EQ(map.TryFind(2)->val, 2);
}

TEST(HashMapTests, ConstIteratorOperator) {
  const HashMap<int32_t, int32_t> map{{1, 2}, {2, 3}};
  auto iter = map.TryFind(1);
  EXPECT_EQ(iter->key, 1);
  EXPECT_EQ(iter->val, 2);
  EXPECT_EQ((*iter).key, 1);  // NOLINT: Test iter
  EXPECT_EQ((*iter).val, 2);  // NOLINT: Test iter

  ++iter;
  EXPECT_EQ(iter, map.TryFind(2));
  EXPECT_EQ(iter->key, 2);
  EXPECT_EQ(iter->val, 3);

  iter++;  // NOLINT: Test iter
  EXPECT_EQ(iter, nullptr);
  EXPECT_FALSE(iter);
}

TEST(HashMapTests, IteratorOperator) {
  HashMap<int32_t, int32_t> map{{1, 2}, {2, 3}};
  auto iter = map.TryFind(1);
  EXPECT_EQ(iter->key, 1);
  EXPECT_EQ(iter->val, 2);
  EXPECT_EQ((*iter).key, 1);  // NOLINT: Test iter
  EXPECT_EQ((*iter).val, 2);  // NOLINT: Test iter

  ++iter;
  EXPECT_EQ(iter, map.TryFind(2));
  EXPECT_EQ(iter->key, 2);
  EXPECT_EQ(iter->val, 3);

  iter++;  // NOLINT: Test iter
  EXPECT_EQ(iter, nullptr);
  EXPECT_FALSE(iter);
}
