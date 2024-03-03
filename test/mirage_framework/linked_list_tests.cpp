#include <gtest/gtest.h>

#include "mirage_framework/base/container/singly_linked_list.hpp"

using namespace mirage;

TEST(SinglyLinkedListTests, Iteration) {
  EXPECT_TRUE(std::forward_iterator<SinglyLinkedList<int32_t>::Iterator>);
  EXPECT_TRUE(std::forward_iterator<SinglyLinkedList<int32_t>::ConstIterator>);

  SinglyLinkedList<int32_t> list = {0, 1, 2};
  int cnt = 0;
  for (auto num : list) {
    EXPECT_EQ(cnt, num);
    ++cnt;
  }
}
