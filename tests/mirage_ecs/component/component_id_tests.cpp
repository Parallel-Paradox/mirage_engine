#include <gtest/gtest.h>

#include "mirage_ecs/component/component_id.hpp"
#include "mirage_ecs/util/marker.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

struct TestComponent : Component {};

struct Counter : Component {
  int32_t *move_cnt_{nullptr};
  int32_t *destruct_cnt_{nullptr};

  Counter() = default;
  Counter(int32_t *move_cnt, int32_t *destruct_cnt)
      : move_cnt_(move_cnt), destruct_cnt_(destruct_cnt) {}

  Counter(Counter &&other) noexcept
      : move_cnt_(other.move_cnt_), destruct_cnt_(other.destruct_cnt_) {
    other.move_cnt_ = nullptr;
    other.destruct_cnt_ = nullptr;
    if (move_cnt_) {
      *move_cnt_ += 1;
    }
  }

  ~Counter() {
    if (destruct_cnt_) {
      *destruct_cnt_ += 1;
    }
    move_cnt_ = nullptr;
    destruct_cnt_ = nullptr;
  }
};

TEST(ComponentIdTests, Consistent) {
  const ComponentId id = ComponentId::Of<TestComponent>();
  EXPECT_EQ(id, ComponentId::Of<TestComponent>());
  EXPECT_EQ(id.type_id(), TypeId::Of<TestComponent>());

  EXPECT_NE(id, ComponentId::Of<Counter>());
}

TEST(ComponentIdTests, MoveAndDestruct) {
  int32_t move_cnt = 0;
  int32_t destruct_cnt = 0;
  auto counter = Counter{&move_cnt, &destruct_cnt};
  const ComponentId id = ComponentId::Of<Counter>();

  Counter move_counter;
  id.move_func()(&counter, &move_counter);
  EXPECT_EQ(move_cnt, 1);
  EXPECT_EQ(destruct_cnt, 0);
  EXPECT_EQ(counter.move_cnt_, nullptr);
  EXPECT_EQ(counter.destruct_cnt_, nullptr);

  id.destruct_func()(&move_counter);
  EXPECT_EQ(move_cnt, 1);
  EXPECT_EQ(destruct_cnt, 1);
  EXPECT_EQ(move_counter.move_cnt_, nullptr);
  EXPECT_EQ(move_counter.destruct_cnt_, nullptr);
}
