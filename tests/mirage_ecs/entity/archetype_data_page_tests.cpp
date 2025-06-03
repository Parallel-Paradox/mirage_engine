#include <gtest/gtest.h>

#include "mirage_ecs/entity/archetype_data_page.hpp"
#include "mirage_ecs/util/marker.hpp"

using namespace mirage::ecs;

struct Counter : Component {
  int32_t *move_cnt_{nullptr};
  int32_t *destruct_cnt_{nullptr};

  Counter() = default;
  Counter(int32_t *move_cnt, int32_t *destruct_cnt)
      : move_cnt_(move_cnt), destruct_cnt_(destruct_cnt) {}

  Counter(Counter &&other) noexcept
      : move_cnt_(other.move_cnt_), destruct_cnt_(other.destruct_cnt_) {
    if (move_cnt_) {
      *move_cnt_ += 1;
    }
    other.move_cnt_ = nullptr;
    other.destruct_cnt_ = nullptr;
  }

  ~Counter() {
    if (destruct_cnt_) {
      *destruct_cnt_ += 1;
    }
    move_cnt_ = nullptr;
    destruct_cnt_ = nullptr;
  }
};

TEST(ArchetypeDataPageTests, Initialize) {
  auto page = ArchetypeDataPage(1024);
  EXPECT_FALSE(page.is_valid());
  EXPECT_EQ(page.capacity(), 0);
  EXPECT_EQ(page.size(), 0);
  EXPECT_NE(page.buffer_ptr(), nullptr);
  EXPECT_EQ(page.align_padding(), 0);
  EXPECT_EQ(page.byte_size(), 1024);
}

TEST(ArchetypeDataPageTests, Push) {
  // TODO
}

TEST(ArchetypeDataPageTests, Remove) {
  // TODO
}

TEST(ArchetypeDataPageTests, View) {
  // TODO
}

TEST(ArchetypeDataPageTests, Slice) {
  // TODO
}
