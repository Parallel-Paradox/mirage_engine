#include <gtest/gtest.h>

#include <thread>

#include "mirage_base/auto_ptr/weak.hpp"

using namespace mirage::base;

namespace {

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

}  // namespace

TEST(WeakTests, Construct) {
  int32_t is_destructed = 0;

  // Default Construct
  WeakAsync<Base> weak;
  EXPECT_EQ(weak, nullptr);

  // From Shared
  auto shared = SharedAsync<Base>::New(&is_destructed);
  EXPECT_EQ(shared.ref_cnt(), 1);
  EXPECT_EQ(shared.weak_ref_cnt(), 0);
  weak = WeakAsync<Base>(shared);
  EXPECT_TRUE(weak);
  EXPECT_EQ(shared.ref_cnt(), 1);
  EXPECT_EQ(shared.weak_ref_cnt(), 1);
  EXPECT_EQ(weak.ref_cnt(), 1);
  EXPECT_EQ(weak.weak_ref_cnt(), 1);

  // From nullptr
  weak = nullptr;
  EXPECT_TRUE(weak.is_null());
  EXPECT_EQ(shared.ref_cnt(), 1);
  EXPECT_EQ(shared.weak_ref_cnt(), 0);
  EXPECT_EQ(is_destructed, 0);
}

TEST(WeakTests, Reset) {
  int32_t is_destructed = 0;
  auto shared = SharedAsync<Base>::New(&is_destructed);
  auto weak = WeakAsync<Base>(shared);
  EXPECT_EQ(is_destructed, 0);

  weak.Reset();
  EXPECT_TRUE(weak.is_null());
  EXPECT_EQ(shared.ref_cnt(), 1);
  EXPECT_EQ(shared.weak_ref_cnt(), 0);
  EXPECT_EQ(is_destructed, 0);
}

TEST(WeakTests, CloneAsync) {
  int32_t is_destructed = 0;
  SharedAsync<Base> shared = SharedAsync<Base>::New(&is_destructed);
  WeakAsync<Base> weak(shared);
  std::atomic_bool is_shared_destructed{false};

  auto async_operation = [&weak, &is_shared_destructed]() {
    for (int32_t i = 0; i < 1e5; ++i) {
      weak.Clone();
    }
    while (!is_shared_destructed.load()) {
      // spin lock
    }
    EXPECT_TRUE(weak.is_null());
    EXPECT_TRUE(weak.TryUpgrade().is_null());
    EXPECT_EQ(weak.ref_cnt(), 0);
    EXPECT_EQ(weak.weak_ref_cnt(), 1);
  };

  std::thread async_thread(async_operation);
  for (int32_t i = 0; i < 1e5; ++i) {
    weak.Clone();
  }

  EXPECT_FALSE(weak.TryUpgrade().is_null());
  EXPECT_EQ(is_destructed, 0);
  shared = nullptr;
  is_shared_destructed.store(true);
  EXPECT_EQ(is_destructed, 1);

  async_thread.join();
}

TEST(WeakTests, ConvertDeriveToBase) {
  int32_t base_destructed = 0;
  int32_t derive_destructed = 0;

  // Convert from derive to base is always successful.
  auto derive_shared =
      SharedLocal<Derive>::New(&base_destructed, &derive_destructed);
  auto derive = WeakLocal<Derive>(derive_shared);
  WeakLocal<Base> base = std::move(derive).Convert<Base>();
  EXPECT_TRUE(derive.is_null());  // NOLINT(*-use-after-move): Allow for test.
  EXPECT_FALSE(base.is_null());
}

TEST(WeakTests, ConvertBaseToDerive) {
  // Can't convert from base to derive when base is the origin type.
  int32_t base_destructed = 0;
  auto base_shared = SharedLocal<Base>::New(&base_destructed);
  auto base = WeakLocal<Base>(base_shared);
  const WeakLocal<Derive> derive_from_base =
      std::move(base).TryConvert<Derive>();
  EXPECT_TRUE(derive_from_base.is_null());
  EXPECT_FALSE(base.is_null());  // NOLINT: Use after move.

  // Convert from base to derive when derive is the origin type.
  int32_t derive_destructed = 0;
  auto derive_shared =
      SharedLocal<Derive>::New(&base_destructed, &derive_destructed);
  auto derive = WeakLocal<Derive>(derive_shared);
  WeakLocal<Base> base_from_derive = std::move(derive).TryConvert<Base>();
  derive = std::move(base_from_derive).TryConvert<Derive>();
  EXPECT_FALSE(derive.is_null());
  EXPECT_TRUE(base_from_derive.is_null());  // NOLINT: Use after move.
}
