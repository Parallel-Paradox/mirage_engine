#include <gtest/gtest.h>

#include "mirage_framework/task/packed_task.hpp"

using namespace mirage;

struct CountUpdate : public Task {
  uint32_t* cnt_{nullptr};
  bool* stop_{nullptr};

  explicit CountUpdate(uint32_t* cnt, bool* stop) : cnt_(cnt), stop_(stop) {}

  Task::Status Update() override {
    if (*stop_) {
      return Task::Done;
    }
    *cnt_ += 1;
    return Task::Running;
  }
};

TEST(TaskTests, ExecutePackedTask) {
  uint32_t cnt = 0;
  bool stop = false;
  PackedTask task(Owned<CountUpdate>::New(&cnt, &stop).TryConvert<Task>());
  EXPECT_EQ(task.GetStatus(), Task::Running);

  task.Update();
  EXPECT_EQ(cnt, 1);
  EXPECT_EQ(task.GetStatus(), Task::Running);

  stop = true;
  task.Update();
  EXPECT_EQ(cnt, 1);
  EXPECT_EQ(task.GetStatus(), Task::Done);
}
