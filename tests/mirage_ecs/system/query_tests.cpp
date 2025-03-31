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
  bool is_component_ref_list = IsComponentRefList<Position&, const Velocity&>;
  EXPECT_TRUE(is_component_ref_list);
  is_component_ref_list = IsComponentRefList<int32_t&>;
  EXPECT_FALSE(is_component_ref_list);
  is_component_ref_list = IsComponentRefList<Position>;
  EXPECT_FALSE(is_component_ref_list);

  bool is_component_list = IsComponentList<Position, Velocity>;
  EXPECT_TRUE(is_component_list);
  is_component_list = IsComponentList<int32_t>;
  EXPECT_FALSE(is_component_list);

  bool is_query_params_list =
      IsQueryParamsList<Ref<Position&, const Velocity&>, With<WithTag>,
                        Without<WithoutTag>>;
  EXPECT_TRUE(is_query_params_list);
  is_query_params_list = IsQueryParamsList<int32_t>;
  EXPECT_FALSE(is_query_params_list);
}

TEST(QueryTests, ExtractQuery) {
  using ValidQuery = Query<Ref<Position&, const Velocity&>, With<WithTag>,
                           Without<WithoutTag>>;
  EXPECT_TRUE(ExtractType<ValidQuery>);
}
