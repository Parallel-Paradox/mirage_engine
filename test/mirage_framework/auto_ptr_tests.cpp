#include <gtest/gtest.h>

#include "mirage_framework/base/auto_ptr/owned.hpp"

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
    Owned<int32_t> raw_owned =
        Owned<int32_t>(&x, [](int32_t* ptr) { *ptr += 1; });
    owned = std::move(raw_owned);
    EXPECT_EQ(owned.Get(), &x);
  }
  EXPECT_EQ(x, 0);  // Destructor won't be called since raw_owned is moved.

  {
    // Move Construct
    Owned<int32_t> move_owned = std::move(owned);
    EXPECT_EQ(*move_owned, 0);
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