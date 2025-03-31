#include <gtest/gtest.h>

#include "mirage_ecs/system/query.hpp"

using namespace mirage::ecs;

TEST(QueryTests, Extract) {
  using ValidQuery =
      Query<Ref<int32_t&, const size_t&>, With<int32_t>, Without<bool>>;
  EXPECT_TRUE(ExtractType<ValidQuery>);
}
