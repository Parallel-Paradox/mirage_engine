#include <gtest/gtest.h>

#include <thread>

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/container/array.hpp"

using namespace mirage::base;

struct Base {
  int32_t* base_destructed{nullptr};

  explicit Base(int32_t* base_destructed) : base_destructed(base_destructed) {}

  virtual ~Base() { *base_destructed += 1; }
};

struct Derive final : Base {
  int32_t* derive_destructed{nullptr};

  explicit Derive(int32_t* base_destructed, int32_t* derive_destructed)
      : Base(base_destructed), derive_destructed(derive_destructed) {}

  ~Derive() override { *derive_destructed += 1; }
};

TEST(SharedPtrTests, Construct) {
  int32_t is_destructed = 0;

  // Default construct
  SharedAsync<Base> shared;
  EXPECT_TRUE(shared.is_null());

  // List construct
  shared = SharedAsync<Base>::New(&is_destructed);
  EXPECT_FALSE(shared.is_null());
  EXPECT_EQ(shared.ref_cnt(), 1);
  EXPECT_EQ(shared.weak_ref_cnt(), 0);

  // Raw construct
  const auto base = new Base(&is_destructed);
  SharedAsync<Base> raw_shared(base);
  EXPECT_EQ(raw_shared.raw_ptr(), base);
  EXPECT_EQ(raw_shared.ref_cnt(), 1);
  EXPECT_EQ(raw_shared.weak_ref_cnt(), 0);

  shared = raw_shared.Clone();
  EXPECT_EQ(shared.ref_cnt(), 2);
  EXPECT_EQ(is_destructed, 1);  // List constructed shared is freed.

  raw_shared.Reset();
  EXPECT_EQ(is_destructed, 1);
  EXPECT_EQ(shared.raw_ptr(), base);
  EXPECT_EQ(shared.ref_cnt(), 1);
  EXPECT_EQ(shared.weak_ref_cnt(), 0);

  // Move construct
  SharedAsync<Base> move_shared(std::move(shared));
  EXPECT_EQ(move_shared.raw_ptr(), base);
  EXPECT_TRUE(shared.is_null());  // NOLINT: Use after move.

  move_shared.Reset();
  EXPECT_EQ(is_destructed, 2);  // Destructor is called.
}

TEST(SharedPtrTests, PtrOps) {
  int32_t cnt = 0;
  const auto ptr = SharedLocal<Base>::New(&cnt);
  EXPECT_EQ(*ptr->base_destructed, 0);

  *ptr->base_destructed = true;
  EXPECT_EQ(*ptr->base_destructed, 1);
}

TEST(SharedPtrTests, CloneAsync) {
  int32_t cnt = 0;
  const auto ptr = SharedAsync<Base>::New(&cnt);

  auto async_operation = [&ptr] {
    Array<SharedAsync<Base>> array;
    for (int32_t i = 0; i < 1e5; ++i) {
      array.Emplace(ptr.Clone());
    }
  };
  std::thread async_thread(async_operation);
  async_operation();
  async_thread.join();
  EXPECT_EQ(cnt, 0);
  EXPECT_EQ(ptr.ref_cnt(), 1);
}

TEST(SharedPtrTests, ConvertBaseToDerive) {
  int32_t base_destructed = 0;
  int32_t derive_destructed = 0;

  // Convert from derive to base is always successful.
  auto derive = SharedLocal<Derive>::New(&base_destructed, &derive_destructed);
  SharedLocal<Base> base = std::move(derive).Convert<Base>();
  EXPECT_TRUE(derive.is_null());  // NOLINT(*-use-after-move): Allow for test.
  EXPECT_FALSE(base.is_null());
  base.Reset();

  // Even holder is base, derive destructor is still called.
  EXPECT_EQ(base_destructed, 1);
  EXPECT_EQ(derive_destructed, 1);
}

TEST(SharedPtrTests, ConvertDeriveToBase) {
  // Can't convert from base to derive when base is the origin type.
  int32_t base_destructed = 0;
  auto base = SharedLocal<Base>::New(&base_destructed);
  const SharedLocal<Derive> derive_from_base =
      std::move(base).TryConvert<Derive>();
  EXPECT_TRUE(derive_from_base.is_null());
  EXPECT_FALSE(base.is_null());  // NOLINT: Use after move.
  EXPECT_EQ(base_destructed, 0);

  // Convert from base to derive when derive is the origin type.
  int32_t derive_destructed = 0;
  auto derive = SharedLocal<Derive>::New(&base_destructed, &derive_destructed);
  SharedLocal<Base> base_from_derive = std::move(derive).TryConvert<Base>();
  derive = std::move(base_from_derive).TryConvert<Derive>();
  EXPECT_FALSE(derive.is_null());
  EXPECT_TRUE(base_from_derive.is_null());  // NOLINT: Use after move.
  EXPECT_EQ(base_destructed, 0);
  EXPECT_EQ(derive_destructed, 0);
}
