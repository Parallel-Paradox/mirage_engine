#include <gtest/gtest.h>

#include "mirage_base/auto_ptr/owned.hpp"

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

TEST(OwnedTests, Construct) {
  int32_t is_destructed = 0;

  // Default Construct
  Owned<Base> owned;
  EXPECT_EQ(owned, nullptr);

  // List Construct
  owned = Owned<Base>::New(&is_destructed);
  EXPECT_FALSE(owned.is_null());

  // Raw construct and set with move
  const auto base = new Base(&is_destructed);
  auto raw_owned = Owned(base);
  owned = std::move(raw_owned);
  EXPECT_EQ(owned.raw_ptr(), base);
  EXPECT_FALSE(raw_owned);  // NOLINT: Use after move.

  EXPECT_EQ(is_destructed, 1);
  raw_owned = nullptr;  // NOLINT: Test nullptr setter
  EXPECT_EQ(is_destructed, 1);

  // Move Construct
  auto move_owned = Owned(std::move(owned));
  EXPECT_EQ(move_owned->base_destructed, &is_destructed);
  EXPECT_TRUE(owned.is_null());  // NOLINT: Use after move.

  EXPECT_EQ(is_destructed, 1);
  move_owned = nullptr;  // NOLINT: Test nullptr setter
  EXPECT_EQ(is_destructed, 2);
}

TEST(OwnedTests, PtrOps) {
  int32_t cnt = 0;
  const auto ptr = Owned<Base>::New(&cnt);
  EXPECT_EQ(*ptr->base_destructed, 0);

  *ptr->base_destructed = true;
  EXPECT_EQ(*ptr->base_destructed, 1);
}

TEST(OwnedTests, ConvertDeriveToBase) {
  int32_t base_destructed = 0;
  int32_t derive_destructed = 0;

  // Convert from derive to base is always successful.
  auto derive = Owned<Derive>::New(&base_destructed, &derive_destructed);
  Owned<Base> base = std::move(derive).Convert<Base>();
  EXPECT_TRUE(derive.is_null());  // NOLINT(*-use-after-move): Allow for test.
  EXPECT_FALSE(base.is_null());
  base.Reset();

  // Even holder is base, derive destructor is still called.
  EXPECT_EQ(base_destructed, 1);
  EXPECT_EQ(derive_destructed, 1);
}

TEST(OwnedTests, ConvertBaseToDerive) {
  // Can't convert from base to derive when base is the origin type.
  int32_t base_destructed = 0;
  auto base = Owned<Base>::New(&base_destructed);
  const Owned<Derive> derive_from_base = std::move(base).TryConvert<Derive>();
  EXPECT_TRUE(derive_from_base.is_null());
  EXPECT_FALSE(base.is_null());  // NOLINT: Use after move.
  EXPECT_EQ(base_destructed, 0);

  // Convert from base to derive when derive is the origin type.
  int32_t derive_destructed = 0;
  auto derive = Owned<Derive>::New(&base_destructed, &derive_destructed);
  Owned<Base> base_from_derive = std::move(derive).TryConvert<Base>();
  derive = std::move(base_from_derive).TryConvert<Derive>();
  EXPECT_FALSE(derive.is_null());
  EXPECT_TRUE(base_from_derive.is_null());  // NOLINT: Use after move.
  EXPECT_EQ(base_destructed, 0);
  EXPECT_EQ(derive_destructed, 0);
}
