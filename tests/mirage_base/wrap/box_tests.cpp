#include <gtest/gtest.h>

#include "mirage_base/wrap/box.hpp"

using namespace mirage::base;

struct UnMovable {
  UnMovable() = default;
  UnMovable(UnMovable&&) = delete;
};

struct alignas(alignof(void*) << 1) BigAlign {};

struct DestructCnt {
  int32_t* destruct_cnt{nullptr};

  DestructCnt(int32_t* destruct_cnt) : destruct_cnt(destruct_cnt) {}

  DestructCnt(DestructCnt&& other) : destruct_cnt(other.destruct_cnt) {
    other.destruct_cnt = nullptr;
  }

  ~DestructCnt() {
    if (destruct_cnt) {
      *destruct_cnt += 1;
    }
  }
};

struct Vec4 {
  size_t x, y, z, w;
  DestructCnt destruct_cnt;

  Vec4(size_t x, size_t y, size_t z, size_t w, DestructCnt destruct_cnt)
      : x(x), y(y), z(z), w(w), destruct_cnt(std::move(destruct_cnt)) {}

  Vec4(Vec4&& other)
      : x(other.x),
        y(other.y),
        z(other.z),
        w(other.w),
        destruct_cnt(std::move(other.destruct_cnt)) {}

  bool operator==(const Vec4& rhs) const {
    return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
  }
};

TEST(BoxTests, SooCondition) {
  EXPECT_TRUE(Box::AllowSmallObjectOptimize<size_t>());
  EXPECT_FALSE(Box::AllowSmallObjectOptimize<UnMovable>());
  EXPECT_FALSE(Box::AllowSmallObjectOptimize<BigAlign>());
  EXPECT_FALSE(Box::AllowSmallObjectOptimize<Vec4>());
}

TEST(BoxTests, AssignValue) {
  Box box;
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
  box = Vec4{1, 2, 3, 4, {&destruct_cnt}};
  EXPECT_TRUE(box.is_valid());
  EXPECT_EQ(box.type_id(), TypeId::Of<Vec4>());
  auto expect_vec = Vec4{1, 2, 3, 4, {&destruct_cnt}};
  EXPECT_EQ(*box.TryCast<Vec4>(), expect_vec);
}

TEST(BoxTests, ResetSmallObject) {
  int32_t destruct_cnt = 0;
  auto box = Box::New(DestructCnt{&destruct_cnt});
  EXPECT_TRUE(box.is_valid());
  EXPECT_EQ(destruct_cnt, 0);
  box.Reset();
  EXPECT_FALSE(box.is_valid());
  EXPECT_EQ(destruct_cnt, 1);
}

TEST(BoxTests, ResetLargeObject) {
  int32_t destruct_cnt = 0;
  auto box = Box::New(Vec4{1, 2, 3, 4, {&destruct_cnt}});
  EXPECT_TRUE(box.is_valid());
  EXPECT_EQ(destruct_cnt, 0);
  box.Reset();
  EXPECT_FALSE(box.is_valid());
  EXPECT_EQ(destruct_cnt, 1);
}
