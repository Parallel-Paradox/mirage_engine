#include <gtest/gtest.h>

#include "mirage_ecs/component/component_handler.hpp"
#include "mirage_ecs/util/marker.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

namespace {

struct TestComponent {
  MIRAGE_COMPONENT;
};

struct Counter {
  MIRAGE_COMPONENT;

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

}  // namespace

TEST(ComponentHandlerTests, Consistent) {
  const ComponentHandler handler = ComponentHandler::Of<TestComponent>();
  EXPECT_EQ(handler, ComponentHandler::Of<TestComponent>());
  EXPECT_EQ(handler.type_id(), TypeId::Of<TestComponent>());
  EXPECT_NE(handler, ComponentHandler::Of<Counter>());
}

TEST(ComponentHandlerTests, MoveAndDestruct) {
  int32_t move_cnt = 0;
  int32_t destruct_cnt = 0;
  auto counter = Counter{&move_cnt, &destruct_cnt};
  const ComponentHandler handler = ComponentHandler::Of<Counter>();

  Counter move_counter;
  handler.move(&counter, &move_counter);
  EXPECT_EQ(move_cnt, 1);
  EXPECT_EQ(destruct_cnt, 0);
  EXPECT_EQ(counter.move_cnt_, nullptr);
  EXPECT_EQ(counter.destruct_cnt_, nullptr);

  handler.destruct(&move_counter);
  EXPECT_EQ(move_cnt, 1);
  EXPECT_EQ(destruct_cnt, 1);
  EXPECT_EQ(move_counter.move_cnt_, nullptr);
  EXPECT_EQ(move_counter.destruct_cnt_, nullptr);
}
