#include <gtest/gtest.h>

#include "mirage_base/container/hash_set.hpp"

using namespace mirage::base;

TEST(HashSetTests, ListConstruct) {
  const HashSet<int32_t> set = {1, 2, 3};
  EXPECT_EQ(set.GetSize(), 3);
  EXPECT_LT(abs(set.GetMaxLoadFactor() - 1.0f), 1e-5);
  EXPECT_FALSE(set.IsEmpty());
  for (int32_t i = 1; i <= 3; ++i) {
    EXPECT_NE(set.TryFind(i), set.end());
  }
  EXPECT_EQ(set.TryFind(4), set.end());
}

TEST(HashSetTests, ExtendAndRehash) {
  HashSet<int32_t> set;

  EXPECT_EQ(set.GetBucketSize(), 0);
  set.Insert(0);
  EXPECT_EQ(set.GetBucketSize(), 16);

  set.Insert(16);
  EXPECT_EQ(set.GetBucketSize(), 16);

  set.SetMaxLoadFactor(0.1f);
  EXPECT_EQ(set.GetBucketSize(), 32);

  set.Insert(1);
  set.Insert(2);
  EXPECT_EQ(set.GetBucketSize(), 64);
}

struct Mark {
  int32_t val;
  int32_t mark;
  Mark(Mark&&) = default;
  Mark(const int32_t v, const int32_t m) : val(v), mark(m) {}
  bool operator==(const Mark& other) const { return val == other.val; }
};

template <>
struct mirage::base::Hash<Mark> {
  size_t operator()(const Mark& mark) const { return mark.val; }
};

TEST(HashSetTests, Insert) {
  HashSet<Mark> set;
  Optional<Mark> rv = set.Insert(Mark(1, 1));
  EXPECT_FALSE(rv.IsValid());
  EXPECT_EQ(set.TryFind(Mark(1, 1))->mark, 1);

  rv = set.Insert(Mark(1, 2));
  EXPECT_TRUE(rv.IsValid());
  EXPECT_EQ(rv.GetRef().mark, 1);
  EXPECT_EQ(set.GetSize(), 1);
  EXPECT_EQ(set.TryFind(Mark(1, 1))->mark, 2);
}

TEST(HashSetTests, Remove) {
  HashSet<Mark> set;
  Optional<Mark> rv = set.Remove(Mark(1, 1));
  EXPECT_FALSE(rv.IsValid());

  set.Insert(Mark(1, 2));
  rv = set.Remove(Mark(1, 1));
  EXPECT_TRUE(rv.IsValid());
  EXPECT_EQ(rv.GetRef().mark, 2);
  EXPECT_EQ(set.GetSize(), 0);
}

TEST(HashSetTests, Iterate) {
  EXPECT_TRUE(std::forward_iterator<HashSet<int32_t>::Iterator>);
  EXPECT_TRUE(std::forward_iterator<HashSet<int32_t>::ConstIterator>);

  for (const HashSet<int32_t> set = {1, 2, 3}; auto num : set) {
    EXPECT_NE(set.TryFind(num), set.end());
  }
}

TEST(HashSetTests, RemoveByIter) {
  HashSet<size_t> set = {1, 2, 3, 18};
  auto iter = set.begin();
  EXPECT_EQ(iter.Remove(), 1);
  EXPECT_EQ(set.GetSize(), 3);
  EXPECT_EQ(*iter, 18);

  ++iter;
  EXPECT_EQ(iter.Remove(), 2);
  EXPECT_EQ(set.GetSize(), 2);
  EXPECT_EQ(*iter, 3);

  EXPECT_EQ(iter.Remove(), 3);
  EXPECT_EQ(set.GetSize(), 1);
  EXPECT_EQ(iter, set.end());

  EXPECT_NE(set.TryFind(18uz), set.end());
}
