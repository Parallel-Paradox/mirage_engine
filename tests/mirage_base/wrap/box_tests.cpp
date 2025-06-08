#include <gtest/gtest.h>

#include "mirage_base/wrap/box.hpp"

using namespace mirage::base;

namespace {

struct UnMovable {
  UnMovable() = default;
  UnMovable(UnMovable&&) = delete;
};

struct alignas(alignof(void*) << 1) BigAlign {};

struct Base {};
struct DestructCnt : public Base {
  int32_t* destruct_cnt{nullptr};

  explicit DestructCnt(int32_t* destruct_cnt) : destruct_cnt(destruct_cnt) {}

  DestructCnt(DestructCnt&& other) noexcept : destruct_cnt(other.destruct_cnt) {
    other.destruct_cnt = nullptr;
  }

  ~DestructCnt() {
    if (destruct_cnt) {
      *destruct_cnt += 1;
    }
  }
};

struct Vec4 : public Base {
  size_t x, y, z, w;
  DestructCnt destruct_cnt;

  Vec4(const size_t x, const size_t y, const size_t z, const size_t w,
       DestructCnt destruct_cnt)
      : x(x), y(y), z(z), w(w), destruct_cnt(std::move(destruct_cnt)) {}

  Vec4(Vec4&& other) noexcept
      : Vec4(other.x, other.y, other.z, other.w,
             std::move(other.destruct_cnt)) {}

  bool operator==(const Vec4& rhs) const {
    return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
  }
};

}  // namespace

TEST(BoxTests, SooCondition) {
  EXPECT_TRUE(BoxAny::AllowSmallObjectOptimize<size_t>());
  EXPECT_FALSE(BoxAny::AllowSmallObjectOptimize<UnMovable>());
  EXPECT_FALSE(BoxAny::AllowSmallObjectOptimize<BigAlign>());
  EXPECT_FALSE(BoxAny::AllowSmallObjectOptimize<Vec4>());
}

TEST(BoxTests, MoveSmall) {
  int32_t destruct_cnt = 0;
  BoxAny box = DestructCnt{&destruct_cnt};
  BoxAny box_move = std::move(box);
  EXPECT_FALSE(box.is_valid());
  EXPECT_TRUE(box_move.is_valid());
  EXPECT_EQ(box_move.type_id(), TypeId::Of<DestructCnt>());
  EXPECT_EQ(destruct_cnt, 0);
}

TEST(BoxTests, MoveLarge) {
  int32_t destruct_cnt = 0;
  BoxAny box = Vec4{1, 2, 3, 4, DestructCnt{&destruct_cnt}};
  BoxAny box_move = std::move(box);
  EXPECT_FALSE(box.is_valid());
  EXPECT_TRUE(box_move.is_valid());
  EXPECT_EQ(box_move.type_id(), TypeId::Of<Vec4>());
  const auto expect_vec = Vec4{1, 2, 3, 4, DestructCnt{&destruct_cnt}};
  EXPECT_EQ(*box_move.TryCast<Vec4>(), expect_vec);
  EXPECT_EQ(destruct_cnt, 0);
}

TEST(BoxTests, AssignValue) {
  BoxAny box;
  EXPECT_FALSE(box.is_valid());

  box = 1;
  EXPECT_TRUE(box.is_valid());
  EXPECT_EQ(box.type_id(), TypeId::Of<int32_t>());
  EXPECT_EQ(*box.TryCast<int32_t>(), 1);

  box = 1.0;
  EXPECT_TRUE(box.is_valid());
  EXPECT_EQ(box.type_id(), TypeId::Of<decltype(1.0)>());
  EXPECT_EQ(*box.TryCast<decltype(1.0)>(), 1.0);

  int32_t destruct_cnt = 0;
  box = Vec4{1, 2, 3, 4, DestructCnt{&destruct_cnt}};
  EXPECT_TRUE(box.is_valid());
  EXPECT_EQ(box.type_id(), TypeId::Of<Vec4>());
  const auto expect_vec = Vec4{1, 2, 3, 4, DestructCnt{&destruct_cnt}};
  EXPECT_EQ(*box.TryCast<Vec4>(), expect_vec);
}

TEST(BoxTests, ResetSmall) {
  int32_t destruct_cnt = 0;
  BoxAny box = DestructCnt{&destruct_cnt};
  EXPECT_TRUE(box.is_valid());
  EXPECT_EQ(destruct_cnt, 0);
  box.Reset();
  EXPECT_FALSE(box.is_valid());
  EXPECT_EQ(destruct_cnt, 1);
}

TEST(BoxTests, ResetLarge) {
  int32_t destruct_cnt = 0;
  BoxAny box = Vec4{1, 2, 3, 4, DestructCnt{&destruct_cnt}};
  EXPECT_TRUE(box.is_valid());
  EXPECT_EQ(destruct_cnt, 0);
  box.Reset();
  EXPECT_FALSE(box.is_valid());
  EXPECT_EQ(destruct_cnt, 1);
}

TEST(BoxTests, Unwrap) {
  int32_t destruct_cnt = 0;
  BoxAny box = Vec4{1, 2, 3, 4, DestructCnt{&destruct_cnt}};
  EXPECT_TRUE(box.is_valid());

  auto vec = box.Unwrap<Vec4>();
  EXPECT_FALSE(box.is_valid());
  EXPECT_EQ(destruct_cnt, 0);
  const auto expect_vec = Vec4{1, 2, 3, 4, DestructCnt{&destruct_cnt}};
  EXPECT_EQ(vec, expect_vec);
}

TEST(BoxTests, BoxBase) {
  int32_t destruct_cnt = 0;
  Box<Base> box = DestructCnt{&destruct_cnt};
  Box<Base> box_move = std::move(box);
  EXPECT_FALSE(box.is_valid());
  EXPECT_TRUE(box_move.is_valid());
  EXPECT_EQ(box_move.type_id(), TypeId::Of<DestructCnt>());
  EXPECT_EQ(destruct_cnt, 0);
}
