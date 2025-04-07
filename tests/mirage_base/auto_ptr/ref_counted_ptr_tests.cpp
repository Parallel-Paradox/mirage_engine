#include <gtest/gtest.h>

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/auto_ptr/weak.hpp"

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

TEST(SharedPtrTests, SharedConstruct) {
  int32_t is_destructed = 0;

  // Default construct
  SharedAsync<Base> shared;
  EXPECT_TRUE(shared.IsNull());

  // List construct
  shared = SharedAsync<Base>::New(&is_destructed);
  EXPECT_FALSE(shared.IsNull());
  EXPECT_EQ(shared.GetRefCnt(), 1);
  EXPECT_EQ(shared.GetWeakRefCnt(), 0);

  // Raw construct
  const auto base = new Base(&is_destructed);
  {
    const SharedAsync<Base> raw_shared(base);
    EXPECT_EQ(raw_shared.Get(), base);
    EXPECT_EQ(raw_shared.GetRefCnt(), 1);
    EXPECT_EQ(raw_shared.GetWeakRefCnt(), 0);

    shared = raw_shared.Clone();
    EXPECT_EQ(shared.GetRefCnt(), 2);
    EXPECT_EQ(is_destructed, 1);  // List constructed shared is freed.
  }
  EXPECT_EQ(is_destructed, 1);
  EXPECT_EQ(shared.Get(), base);
  EXPECT_EQ(shared.GetRefCnt(), 1);
  EXPECT_EQ(shared.GetWeakRefCnt(), 0);

  // Move construct
  {
    const SharedAsync<Base> move_shared(std::move(shared));
    EXPECT_EQ(move_shared.Get(), base);
    EXPECT_TRUE(shared.IsNull());  // NOLINT: Use after move.
  }
  EXPECT_EQ(is_destructed, 2);  // Destructor is called.
}
