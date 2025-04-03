#include <gtest/gtest.h>

#include <thread>

#include "mirage_base/auto_ptr/ref_count.hpp"

using namespace mirage::base;

TEST(RefCountTests, DefaultCounstruct) {
  EXPECT_TRUE(IsRefCount<RefCountLocal>);
  EXPECT_TRUE(IsRefCount<RefCountAsync>);
  EXPECT_EQ(RefCountLocal().GetCnt(), 1);
  EXPECT_EQ(RefCountAsync().GetCnt(), 1);
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

  RefCountLocal count_local;
  checker(&count_local);

  RefCountAsync count_async;
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

  RefCountLocal count_local;
  checker(&count_local);

  RefCountAsync count_async;
  checker(&count_async);
}

TEST(RefCountTests, Async) {
  RefCountAsync count;
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
