#include <gtest/gtest.h>

#include "mirage_base/auto_ptr/owned.hpp"
#include "mirage_base/container/array.hpp"

using namespace mirage::base;

struct Counter final {
  int32_t* base_destructed{nullptr};

  explicit Counter(int32_t* base_destructed)
      : base_destructed(base_destructed) {}

  ~Counter() { *base_destructed += 1; }
};

TEST(ArrayTests, Construct) {
  Array<int32_t> array = {0, 1, 2};

  const Array<int32_t> copy_array(array);
  EXPECT_EQ(array, copy_array);

  int32_t* raw_ptr = array.data();
  const Array<int32_t> move_array(std::move(array));
  EXPECT_TRUE(array.empty());  // NOLINT(*-use-after-move): Allow for test.
  EXPECT_EQ(array.data(), nullptr);
  EXPECT_EQ(raw_ptr, move_array.data());
  EXPECT_EQ(move_array, copy_array);
}

TEST(ArrayTests, DestructAfterMoved) {
  int32_t destruct_cnt = 0;
  {
    Array<Owned<Counter>> src;
    src.Emplace(Owned<Counter>::New(&destruct_cnt));
    src.Emplace(Owned<Counter>::New(&destruct_cnt));
    Array<Owned<Counter>> dst(std::move(src));
    Owned<Counter> pop = dst.Pop();
    EXPECT_EQ(destruct_cnt, 0);
  }
  EXPECT_EQ(destruct_cnt, 2);
}

TEST(ArrayTests, ChangeSizeAndCapacity) {
  Array<int32_t> array = {0, 1, 2};
  EXPECT_EQ(array.size(), 3);
  EXPECT_EQ(array.capacity(), 3);

  array.Push(3);
  EXPECT_EQ(array.size(), 4);
  EXPECT_EQ(array.capacity(), 6);

  array.Reserve(5);
  EXPECT_EQ(array.size(), 4);
  EXPECT_EQ(array.capacity(), 6);

  array.set_capacity(5);
  EXPECT_EQ(array.size(), 4);
  EXPECT_EQ(array.capacity(), 5);

  array.set_size(5);
  EXPECT_EQ(array.size(), 5);
  EXPECT_EQ(array[4], 0);
  EXPECT_EQ(array.capacity(), 5);

  array.set_size(3);
  EXPECT_EQ(array.size(), 3);
  EXPECT_EQ(array.capacity(), 5);

  array.ShrinkToFit();
  EXPECT_EQ(array.size(), 3);
  EXPECT_EQ(array.capacity(), 3);
}

TEST(ArrayTests, CompareEquality) {
  const Array<int32_t> array_a = {0, 1, 2};
  const Array<int32_t> array_b = {2, 1, 0};
  EXPECT_EQ(array_a, array_a);
  EXPECT_NE(array_a, array_b);
}

TEST(ArrayTests, IterateArray) {
  EXPECT_TRUE(std::contiguous_iterator<Array<int32_t>::Iterator>);
  EXPECT_TRUE(std::contiguous_iterator<Array<int32_t>::ConstIterator>);
  Array<int32_t> array = {0, 1, 2};
  for (auto iter = array.begin(); iter != array.end(); ++iter) {  // NOLINT
    *iter += 1;
  }
  for (auto& num : array) {
    num += 1;
  }
  const Array compare = {2, 3, 4};
  const auto arr_iter = array.begin();
  const auto cmp_iter = compare.begin();
  for (int32_t i = 0; i < 3; ++i) {
    EXPECT_EQ(arr_iter[i], cmp_iter[i]);
  }
}

TEST(ArrayTests, Insert) {
  Array<int32_t> array = {0, 1, 2};
  array.Insert(1, 3);
  Array<int32_t> result = {0, 3, 1, 2};
  EXPECT_EQ(array, result);

  array.Insert(0, 4);
  result = {4, 0, 3, 1, 2};
  EXPECT_EQ(array, result);
}

TEST(ArrayTests, Remove) {
  Array<int32_t> array = {0, 1, 2, 3};
  Array<int32_t> expect_array;

  array.SwapRemove(0);
  expect_array = {3, 1, 2};
  EXPECT_EQ(array, expect_array);

  array.Remove(0);
  expect_array = {1, 2};
  EXPECT_EQ(array, expect_array);
}

TEST(ArrayTests, Swap) {
  Array<int32_t> array = {0, 1, 2};
  Array<int32_t> expect_array;

  array.Swap(0, 0);
  expect_array = {0, 1, 2};
  EXPECT_EQ(array, expect_array);

  array.Swap(0, 1);
  expect_array = {1, 0, 2};
  EXPECT_EQ(array, expect_array);
}
