#include <gtest/gtest.h>
#include <memory>
#include <thread>
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

TEST(AsyncQueueTests, PopFromEmpty) {
  AsyncQueue<int32_t> queue;
  EXPECT_EQ(queue.Pop(), std::nullopt);
}

TEST(AsyncQueueTests, AsyncEmplace) {
  auto worker = [](AsyncQueue<int32_t>& queue) {
    for (int i = 0; i < 100; ++i) {
      queue.Emplace(0);
    }
  };
  AsyncQueue<int32_t> queue;
  std::thread worker_a(worker, std::ref(queue));
  std::thread worker_b(worker, std::ref(queue));
  worker_a.join();
  worker_b.join();
  EXPECT_EQ(queue.GetSize(), 200);
}
