#include <gtest/gtest.h>

#include "mirage_ecs/system/query.hpp"

using namespace mirage::ecs;

struct Position : Component {
  float x;
  float y;
};

struct Velocity : Component {
  float x;
  float y;
};

struct WithTag : Component {};

struct WithoutTag : Component {};

TEST(QueryTests, ParamsConceptCheck) {
  bool is_component_ref = IsComponentRef<Position&, const Velocity&>;
  EXPECT_TRUE(is_component_ref);
  is_component_ref = IsComponentRef<int32_t&>;
  EXPECT_FALSE(is_component_ref);
  is_component_ref = IsComponentRef<Position>;
  EXPECT_FALSE(is_component_ref);

  bool is_component = IsComponent<Position, Velocity>;
  EXPECT_TRUE(is_component);
  is_component = IsComponent<int32_t>;
  EXPECT_FALSE(is_component);

  bool is_query_param = IsQueryParam<Ref<Position&, const Velocity&>,
                                     With<WithTag>, Without<WithoutTag>>;
  EXPECT_TRUE(is_query_param);
  is_query_param = IsQueryParam<int32_t>;
  EXPECT_FALSE(is_query_param);
}

TEST(QueryTests, ExtractQuery) {
  using ValidQuery = Query<Ref<Position&, const Velocity&>, With<WithTag>,
                           Without<WithoutTag>>;
  EXPECT_TRUE(ExtractType<ValidQuery>);

  constexpr bool ref_checker =
      std::same_as<ValidQuery::RefTypeList,
                   std::tuple<Position&, const Velocity&>>;
  EXPECT_TRUE(ref_checker);

  constexpr bool with_checker =
      std::same_as<ValidQuery::WithTypeList, std::tuple<WithTag>>;
  EXPECT_TRUE(with_checker);

  constexpr bool without_checker =
      std::same_as<ValidQuery::WithoutTypeList, std::tuple<WithoutTag>>;
  EXPECT_TRUE(without_checker);
}

TEST(QueryTests, ComponentList) {
  bool same_type =
      std::same_as<Position, ComponentList<Position, Velocity>::Get<0>::Type>;
  EXPECT_TRUE(same_type);

  same_type =
      std::same_as<Velocity, ComponentList<Position, Velocity>::Get<1>::Type>;
  EXPECT_TRUE(same_type);
}
