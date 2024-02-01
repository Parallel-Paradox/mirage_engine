#include <gtest/gtest.h>
#include <sys/_types/_int32_t.h>

#include <atomic>
#include <thread>

#include "mirage_framework/base/auto_ptr/owned.hpp"
#include "mirage_framework/base/auto_ptr/ref_count.hpp"
#include "mirage_framework/base/auto_ptr/shared.hpp"
#include "mirage_framework/base/auto_ptr/weak.hpp"

using namespace mirage;

TEST(AutoPtrTests, OwnedConstruct) {
  // Default Construct
  Owned<int32_t> owned;
  EXPECT_TRUE(owned.IsNull());

  // List Construct
  owned = Owned<int32_t>::New(0);
  EXPECT_FALSE(owned.IsNull());

  // Raw construct
  int32_t x = 0;
  {
    Owned<int32_t> raw_owned(&x, [](int32_t* ptr) { *ptr += 1; });
    owned = std::move(raw_owned);
    EXPECT_EQ(owned.Get(), &x);
    EXPECT_TRUE(raw_owned.IsNull());
  }
  EXPECT_EQ(x, 0);  // Destructor won't be called since raw_owned is moved.

  {
    // Move Construct
    Owned<int32_t> move_owned(std::move(owned));
    EXPECT_EQ(*move_owned, 0);
    EXPECT_TRUE(owned.IsNull());
  }
  EXPECT_EQ(x, 1);  // Destructor will be called since move_owned is destructed.
}

struct Base {
  bool* base_destructed_{nullptr};

  explicit Base(bool* base_destructed) : base_destructed_(base_destructed) {}

  virtual ~Base() { *base_destructed_ = true; }
};

struct Derive : public Base {
  bool* derive_destructed_{nullptr};

  explicit Derive(bool* base_destructed, bool* derive_destructed)
      : Base(base_destructed), derive_destructed_(derive_destructed) {}

  ~Derive() override { *derive_destructed_ = true; }
};

TEST(AutoPtrTests, OwnedConvertDeriveToBase) {
  bool base_destructed = false;
  bool derive_destructed = false;
  {
    // Convert from derive to base is always successful.
    Owned<Derive> derive =
        Owned<Derive>::New(&base_destructed, &derive_destructed);
    Owned<Base> base(derive.TryConvert<Base>());
    EXPECT_TRUE(derive.IsNull());
    EXPECT_FALSE(base.IsNull());
  }
  // Even holder is base, derive destructor is still called.
  EXPECT_TRUE(base_destructed);
  EXPECT_TRUE(derive_destructed);
}

TEST(AutoPtrTests, OwnedConvertBaseToDerive) {
  // Can't convert from base to derive when base is the origin type.
  bool base_destructed = false;
  Owned<Base> base = Owned<Base>::New(&base_destructed);
  EXPECT_TRUE(base.TryConvert<Derive>().IsNull());
  EXPECT_FALSE(base.IsNull());
  EXPECT_FALSE(base_destructed);

  // Convert from base to derive when derive is the origin type.
  bool derive_destructed = false;
  Owned<Derive> derive =
      Owned<Derive>::New(&base_destructed, &derive_destructed);
  Owned<Base> base_from_derive = derive.TryConvert<Base>();
  derive = base_from_derive.TryConvert<Derive>();
  EXPECT_FALSE(derive.IsNull());
  EXPECT_TRUE(base_from_derive.IsNull());
  EXPECT_FALSE(base_destructed);
  EXPECT_FALSE(derive_destructed);
}

TEST(AutoPtrTests, RefCountBehaviour) {
  EXPECT_TRUE(AsRefCount<RefCountLocal>);
  EXPECT_TRUE(AsRefCount<RefCountAsync>);

  auto checker = [](RefCount* count) {
    EXPECT_EQ(count->GetCnt(), 0);

    bool increase = count->TryIncrease();
    EXPECT_FALSE(increase);
    EXPECT_EQ(count->GetCnt(), 0);

    bool release = count->TryRelease();
    EXPECT_TRUE(release);
    EXPECT_EQ(count->GetCnt(), 0);

    count->Increase();
    EXPECT_EQ(count->GetCnt(), 1);

    increase = count->TryIncrease();
    EXPECT_TRUE(increase);
    EXPECT_EQ(count->GetCnt(), 2);

    release = count->TryRelease();
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

TEST(AutoPtrTests, SharedConstruct) {
  // Default construct
  SharedAsync<int32_t> shared;
  EXPECT_TRUE(shared.IsNull());

  // List construct
  shared = SharedAsync<int32_t>::New(0);
  EXPECT_FALSE(shared.IsNull());
  EXPECT_EQ(shared.GetRefCnt(), 1);
  EXPECT_EQ(shared.GetWeakRefCnt(), 0);

  // Raw construct
  int32_t x = 0;
  {
    SharedAsync<int32_t> raw_shared(&x, [](int32_t* ptr) { *ptr += 1; });
    EXPECT_EQ(raw_shared.Get(), &x);
    EXPECT_EQ(raw_shared.GetRefCnt(), 1);
    EXPECT_EQ(raw_shared.GetWeakRefCnt(), 0);

    shared = raw_shared.Clone();
    EXPECT_EQ(shared.GetRefCnt(), 2);
  }
  EXPECT_EQ(x, 0);  // Destructor won't be called since ref_cnt increase.
  EXPECT_EQ(shared.Get(), &x);
  EXPECT_EQ(shared.GetRefCnt(), 1);
  EXPECT_EQ(shared.GetWeakRefCnt(), 0);

  // Move construct
  {
    SharedAsync<int32_t> move_shared(std::move(shared));
    EXPECT_EQ(*move_shared, 0);
    EXPECT_TRUE(shared.IsNull());
  }
  EXPECT_EQ(x, 1);  // Destructor is called.
}

TEST(AutoPtrTests, SharedCloneAsync) {
  int32_t x = 0;
  SharedAsync<int32_t> shared(&x, [](int32_t* ptr) { *ptr += 1; });

  auto async_operation = [&shared]() {
    for (int32_t i = 0; i < 100; ++i) {
      shared.Clone();
    }
  };
  std::thread async_thread(async_operation);
  async_operation();
  async_thread.join();
  EXPECT_EQ(x, 0);
  EXPECT_EQ(shared.GetRefCnt(), 1);
}

TEST(AutoPtrTests, WeakCloneAsync) {
  int32_t x = 0;
  SharedAsync<int32_t> shared(&x, [](int32_t* ptr) { *ptr += 1; });
  WeakAsync<int32_t> weak(shared);
  std::atomic_bool is_shared_destructed{false};

  auto async_operation = [&weak, &is_shared_destructed]() {
    for (int32_t i = 0; i < 100; ++i) {
      weak.Clone();
    }
    while (!is_shared_destructed.load()) {}  // spin lock
    EXPECT_TRUE(weak.IsNull());
    EXPECT_TRUE(weak.TryUpgrade().IsNull());
    EXPECT_EQ(weak.GetRefCnt(), 0);
    EXPECT_EQ(weak.GetWeakRefCnt(), 1);
  };

  std::thread async_thread(async_operation);
  for (int32_t i = 0; i < 100; ++i) {
    weak.Clone();
  }

  EXPECT_FALSE(weak.TryUpgrade().IsNull());
  EXPECT_EQ(x, 0);
  shared = nullptr;
  is_shared_destructed.store(true);
  EXPECT_EQ(x, 1);

  async_thread.join();
}
