#include <gtest/gtest.h>

#include <cstddef>

#include "mirage_ecs/entity/component_package.hpp"
#include "mirage_ecs/util/marker.hpp"
#include "mirage_ecs/util/type_set.hpp"

using namespace mirage;
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

TEST(ComponentPackageTests, AddComponent) {
  ComponentPackage package;
  EXPECT_EQ(package.size(), 0);

  size_t counter1 = 0;
  auto rv1 = package.Add(DestructCounter(&counter1));

  size_t counter2 = 0;
  auto rv2 = package.Add(DestructCounter(&counter2));

  EXPECT_EQ(package.size(), 1);
  EXPECT_TRUE(package.type_set().With(base::TypeId::Of<DestructCounter>()));
  EXPECT_FALSE(rv1.is_valid());
  EXPECT_TRUE(rv2.is_valid());
  EXPECT_EQ(counter1, 0);
  EXPECT_EQ(counter2, 0);

  size_t* counter1_ptr = rv2.Unwrap().counter_ptr;
  EXPECT_EQ(counter1_ptr, &counter1);
  EXPECT_EQ(counter1, 1);
}

TEST(ComponentPackageTests, RemoveComponent) {
  ComponentPackage package;

  auto empty_ev = package.Remove<DestructCounter>();
  EXPECT_FALSE(empty_ev.is_valid());

  size_t counter = 0;
  package.Add(DestructCounter(&counter));

  auto rv = package.Remove<DestructCounter>();
  EXPECT_TRUE(rv.is_valid());
  EXPECT_EQ(counter, 0);
  EXPECT_EQ(package.size(), 0);
  EXPECT_TRUE(package.type_set().Without(base::TypeId::Of<DestructCounter>()));

  size_t* counter_ptr = rv.Unwrap().counter_ptr;
  EXPECT_EQ(counter, 1);
  EXPECT_EQ(counter_ptr, &counter);
}
