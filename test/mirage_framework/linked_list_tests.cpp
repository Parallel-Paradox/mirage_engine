#include <gtest/gtest.h>

#include "mirage_framework/base/container/singly_linked_list.hpp"

using namespace mirage;

TEST(SinglyLinkedListTests, Iterate) {
  using Iter = SinglyLinkedList<int32_t>::Iterator;
  using ConstIter = SinglyLinkedList<int32_t>::ConstIterator;

  EXPECT_TRUE(std::forward_iterator<SinglyLinkedList<int32_t>::Iterator>);
  EXPECT_TRUE(std::forward_iterator<SinglyLinkedList<int32_t>::ConstIterator>);

  SinglyLinkedList<int32_t> list = {0, 1, 2};

  int cnt = 0;
  for (int32_t num : list) {
    EXPECT_EQ(cnt, num);
    ++cnt;
  }

  cnt = 0;
  for (Iter iter = list.begin(); iter != list.end(); ++iter) {
    EXPECT_EQ(cnt, *iter);
    ++cnt;
  }

  cnt = 0;
  const SinglyLinkedList<int32_t>& const_list = list;
  for (ConstIter iter = const_list.begin(); iter != const_list.end(); ++iter) {
    EXPECT_EQ(cnt, *iter);
    ++cnt;
  }
}
