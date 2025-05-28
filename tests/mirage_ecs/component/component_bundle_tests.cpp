#include <gtest/gtest.h>

#include <cstddef>

#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/util/marker.hpp"
#include "mirage_ecs/util/type_set.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

struct DestructCounter : Component {
  size_t* counter_ptr{nullptr};

  DestructCounter(size_t* counter_ptr) : counter_ptr(counter_ptr) {}

  ~DestructCounter() {
    if (counter_ptr) {
      ++(*counter_ptr);
    }
  }

  DestructCounter(DestructCounter&& other) noexcept
      : counter_ptr(other.counter_ptr) {
    other.counter_ptr = nullptr;
  }
};

TEST(ComponentBundleTests, AddComponent) {
  ComponentBundle bundle;
  EXPECT_EQ(bundle.size(), 0);

  size_t counter1 = 0;
  auto rv1 = bundle.Add(DestructCounter(&counter1));

  size_t counter2 = 0;
  auto rv2 = bundle.Add(DestructCounter(&counter2));

  EXPECT_EQ(bundle.size(), 1);
  EXPECT_TRUE(bundle.MakeTypeSet().With(TypeId::Of<DestructCounter>()));
  EXPECT_FALSE(rv1.is_valid());
  EXPECT_TRUE(rv2.is_valid());
  EXPECT_EQ(counter1, 0);
  EXPECT_EQ(counter2, 0);

  size_t* counter1_ptr = rv2.Unwrap().counter_ptr;
  EXPECT_EQ(counter1_ptr, &counter1);
  EXPECT_EQ(counter1, 1);
}

TEST(ComponentBundleTests, RemoveComponent) {
  ComponentBundle bundle;

  auto empty_ev = bundle.Remove<DestructCounter>();
  EXPECT_FALSE(empty_ev.is_valid());

  size_t counter = 0;
  bundle.Add(DestructCounter(&counter));

  auto rv = bundle.Remove<DestructCounter>();
  EXPECT_TRUE(rv.is_valid());
  EXPECT_EQ(counter, 0);
  EXPECT_EQ(bundle.size(), 0);
  EXPECT_TRUE(bundle.MakeTypeSet().Without(TypeId::Of<DestructCounter>()));

  size_t* counter_ptr = rv.Unwrap().counter_ptr;
  EXPECT_EQ(counter, 1);
  EXPECT_EQ(counter_ptr, &counter);
}
