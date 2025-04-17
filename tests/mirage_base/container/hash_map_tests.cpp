#include <gtest/gtest.h>

#include "mirage_base/container/hash_map.hpp"

using namespace mirage::base;

TEST(HashMapTests, ListConstruct) {
  const HashMap<int32_t, int32_t> map{{1, 1}, {2, 2}, {3, 3}};
  EXPECT_FALSE(map.empty());
  EXPECT_EQ(map.size(), 3);
  for (int32_t i = 1; i <= 3; ++i) {
    EXPECT_NE(map.TryFind(i), map.end());
    EXPECT_EQ(map.TryFind(i)->key(), i);
    EXPECT_EQ(map.TryFind(i)->val(), i);
  }
  EXPECT_EQ(map.TryFind(4), map.end());
}

TEST(HashMapTests, Insert) {
  HashMap<int32_t, int32_t> map;
  EXPECT_TRUE(map.empty());
  EXPECT_EQ(map.size(), 0);

  auto rv = map.Insert(1, 1);
  EXPECT_EQ(map.size(), 1);
  EXPECT_FALSE(rv.is_valid());
  EXPECT_EQ(map.TryFind(1)->key(), 1);
  EXPECT_EQ(map.TryFind(1)->val(), 1);

  rv = map.Insert(1, 2);
  EXPECT_EQ(map.size(), 1);
  EXPECT_TRUE(rv.is_valid());

  auto kv = rv.Unwrap();
  EXPECT_EQ(kv.key(), 1);
  EXPECT_EQ(kv.val(), 1);
  EXPECT_EQ(map.TryFind(1)->key(), 1);
  EXPECT_EQ(map.TryFind(1)->val(), 2);
}

TEST(HashMapTests, Remove) {
  HashMap<int32_t, int32_t> map;
  auto rv = map.Remove(1);
  EXPECT_FALSE(rv.is_valid());
  EXPECT_EQ(map.size(), 0);

  map.Insert(1, 1);
  rv = map.Remove(1);
  EXPECT_TRUE(rv.is_valid());

  auto kv = rv.Unwrap();
  EXPECT_EQ(kv.key(), 1);
  EXPECT_EQ(kv.val(), 1);
  EXPECT_EQ(map.size(), 0);
  EXPECT_EQ(map.TryFind(1), map.end());
}

TEST(HashMapTests, Iterate) {
  using Iterator = HashMap<int32_t, int32_t>::Iterator;
  using ConstIterator = HashMap<int32_t, int32_t>::ConstIterator;
  EXPECT_TRUE(std::forward_iterator<Iterator>);
  EXPECT_TRUE(std::forward_iterator<ConstIterator>);

  for (const HashMap<int32_t, int32_t> map{{1, 1}, {2, 2}, {3, 3}};
       auto& kv : map) {
    EXPECT_EQ(kv.key(), kv.val());
    EXPECT_NE(map.TryFind(kv.key()), map.end());
  }
}

TEST(HashMapTests, ConstIteratorOperator) {
  const HashMap<int32_t, int32_t> map{{1, 2}, {2, 3}};
  auto iter = map.TryFind(1);
  EXPECT_EQ(iter->key(), 1);
  EXPECT_EQ(iter->val(), 2);
  EXPECT_EQ((*iter).key(), 1);  // NOLINT: Test iter
  EXPECT_EQ((*iter).val(), 2);  // NOLINT: Test iter

  ++iter;
  EXPECT_EQ(iter, map.TryFind(2));
  EXPECT_EQ(iter->key(), 2);
  EXPECT_EQ(iter->val(), 3);

  iter++;  // NOLINT: Test iter
  EXPECT_EQ(iter, nullptr);
  EXPECT_FALSE(iter);
}

TEST(HashMapTests, IteratorOperator) {
  HashMap<int32_t, int32_t> map{{1, 2}, {2, 3}};
  auto iter = map.TryFind(1);
  EXPECT_EQ(iter->key(), 1);
  EXPECT_EQ(iter->val(), 2);
  EXPECT_EQ((*iter).key(), 1);  // NOLINT: Test iter
  EXPECT_EQ((*iter).val(), 2);  // NOLINT: Test iter

  ++iter;
  EXPECT_EQ(iter, map.TryFind(2));
  EXPECT_EQ(iter->key(), 2);
  EXPECT_EQ(iter->val(), 3);

  iter++;  // NOLINT: Test iter
  EXPECT_EQ(iter, nullptr);
  EXPECT_FALSE(iter);
}
