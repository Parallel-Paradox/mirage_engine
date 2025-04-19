#include <gtest/gtest.h>

#include "mirage_ecs/system/query.hpp"

using namespace mirage;
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
  bool is_component_ref = IsComponentRefList<Position&, const Velocity&>;
  EXPECT_TRUE(is_component_ref);
  is_component_ref = IsComponentRef<int32_t&>;
  EXPECT_FALSE(is_component_ref);
  is_component_ref = IsComponentRef<Position>;
  EXPECT_FALSE(is_component_ref);

  bool is_component = IsComponentList<Position, Velocity>;
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
  EXPECT_TRUE(IsExtractType<ValidQuery>);

  constexpr bool ref_checker =
      std::same_as<ValidQuery::RefTypeList,
                   base::TypeList<Position&, const Velocity&>>;
  EXPECT_TRUE(ref_checker);

  constexpr bool with_checker =
      std::same_as<ValidQuery::WithTypeList, base::TypeList<WithTag>>;
  EXPECT_TRUE(with_checker);

  constexpr bool without_checker =
      std::same_as<ValidQuery::WithoutTypeList, base::TypeList<WithoutTag>>;
  EXPECT_TRUE(without_checker);
}

TEST(QueryTests, ComponentList) {
  using TypeList = base::TypeList<Position, Velocity>;
  bool same_type = std::same_as<Position, TypeList::Get<0>::Type>;
  EXPECT_TRUE(same_type);

  same_type = std::same_as<Velocity, TypeList::Get<1>::Type>;
  EXPECT_TRUE(same_type);
}
