#include <gtest/gtest.h>

#include <thread>

#include "mirage_base/auto_ptr/ref_count.hpp"

using namespace mirage::base;

TEST(RefCountTests, Counstruct) {
  EXPECT_TRUE(IsRefCount<RefCountLocal>);
  EXPECT_TRUE(IsRefCount<RefCountAsync>);
  EXPECT_EQ(RefCountLocal(1).GetCnt(), 1);
  EXPECT_EQ(RefCountAsync(1).GetCnt(), 1);
}

TEST(RefCountTests, ZeroCountBehaviour) {
  auto checker = [](RefCount* count) {
    count->TryRelease();

    const bool increase = count->TryIncrease();
    EXPECT_FALSE(increase);
    EXPECT_EQ(count->GetCnt(), 0);

    const bool release = count->TryRelease();
    EXPECT_TRUE(release);
    EXPECT_EQ(count->GetCnt(), 0);
  };

  RefCountLocal count_local(1);
  checker(&count_local);

  RefCountAsync count_async(1);
  checker(&count_async);
}

TEST(RefCountTests, ResetBehaviour) {
  auto checker = [](RefCount* count) {
    const bool increase = count->TryIncrease();
    EXPECT_TRUE(increase);
    EXPECT_EQ(count->GetCnt(), 2);

    bool release = count->TryRelease();
    EXPECT_FALSE(release);
    EXPECT_EQ(count->GetCnt(), 1);

    release = count->TryRelease();
    EXPECT_TRUE(release);
    EXPECT_EQ(count->GetCnt(), 0);
  };

  RefCountLocal count_local(1);
  checker(&count_local);

  RefCountAsync count_async(1);
  checker(&count_async);
}

TEST(RefCountTests, Async) {
  RefCountAsync count(1);
  auto async_operation = [&count] {
    for (int32_t i = 0; i < 1e5; ++i) {
      count.TryIncrease();
    }
    for (int32_t i = 0; i < 1e5; ++i) {
      count.TryRelease();
    }
  };
  std::thread async_thread(async_operation);
  async_operation();
  async_thread.join();
  EXPECT_EQ(count.GetCnt(), 1);
}
