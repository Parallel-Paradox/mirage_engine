#include <gtest/gtest.h>

#include "mirage_base/util/type_id.hpp"
#include "mirage_ecs/util/type_set.hpp"

using namespace mirage;
using namespace mirage::ecs;

TEST(TypeSetTests, AddType) {
  EXPECT_TRUE(base::IsHashable<TypeSet>);
  auto set = TypeSet::New<int64_t>();

  set.AddType<int32_t>();
  const auto expect_set = TypeSet::New<int64_t, int32_t, int64_t>();
  EXPECT_EQ(set, expect_set);
  EXPECT_EQ(set.type_array(), expect_set.type_array());

  set.AddTypeId(base::TypeId::Of<int32_t>());
  EXPECT_EQ(set, expect_set);
}

TEST(TypeSetTests, RemoveType) {
  auto set = TypeSet::New<int64_t, int32_t>();

  set.RemoveType<int32_t>();
  const auto expect_set = TypeSet::New<int64_t>();
  EXPECT_EQ(set, expect_set);
  EXPECT_EQ(set.type_array(), expect_set.type_array());

  set.RemoveTypeId(base::TypeId::Of<int32_t>());
  EXPECT_EQ(set, expect_set);
}

TEST(TypeSetTests, QueryMatch) {
  const auto set = TypeSet::New<int64_t, int32_t>();
  EXPECT_TRUE(set.With(set));
  EXPECT_FALSE(set.Without(set));

  const auto empty_set = TypeSet();
  EXPECT_TRUE(set.With(empty_set));
  EXPECT_FALSE(empty_set.With(set));
  EXPECT_TRUE(set.Without(empty_set));
  EXPECT_TRUE(empty_set.Without(set));

  const auto with_set_success = TypeSet::New<int64_t>();
  const auto without_set_success = TypeSet::New<bool>();
  EXPECT_TRUE(set.With(with_set_success));
  EXPECT_TRUE(set.Without(without_set_success));
  EXPECT_FALSE(set.With(without_set_success));
  EXPECT_FALSE(set.Without(with_set_success));

  EXPECT_TRUE(set.With(base::TypeId::Of<int64_t>()));
  EXPECT_TRUE(set.Without(base::TypeId::Of<bool>()));

  const auto with_set_fail_0 = TypeSet::New<int64_t, int32_t, bool>();
  const auto with_set_fail_1 = TypeSet::New<int32_t, bool>();
  EXPECT_FALSE(set.With(with_set_fail_0));
  EXPECT_FALSE(set.With(with_set_fail_1));
  EXPECT_FALSE(set.Without(with_set_fail_0));
  EXPECT_FALSE(set.Without(with_set_fail_1));
}
