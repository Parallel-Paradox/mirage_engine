#include <gtest/gtest.h>
#include <memory>
#include "mirage_framework/task/async_queue.hpp"

using namespace mirage;

TEST(AsyncQueueTests, ConstructFromCopyable) {
  AsyncQueue<int32_t> queue;
  EXPECT_EQ(queue.GetSize(), 0);
  EXPECT_TRUE(queue.IsEmpty());

  queue.Push(0);
  AsyncQueue<int32_t> copy_queue(queue);
  EXPECT_EQ(queue.GetSize(), 1);
  EXPECT_EQ(copy_queue.GetSize(), 1);

  AsyncQueue<int32_t> move_queue(std::move(queue));
  EXPECT_EQ(queue.GetSize(), 0);
  EXPECT_EQ(copy_queue.GetSize(), 1);
  EXPECT_EQ(move_queue.GetSize(), 1);
}

TEST(AsyncQueueTests, ExceptionWhenNotCopyable) {
  AsyncQueue<std::unique_ptr<int32_t>> queue;
  EXPECT_ANY_THROW(AsyncQueue<std::unique_ptr<int32_t>> copy_queue(queue));
  EXPECT_ANY_THROW(queue.Push(std::make_unique<int32_t>(0)));
}

// TODO: Async push, emplace and pop
