#ifndef MIRAGE_ECS_SYSTEM_QUERY
#define MIRAGE_ECS_SYSTEM_QUERY

#include <tuple>

#include "mirage_ecs/marker.hpp"
#include "mirage_ecs/system/extract.hpp"

namespace mirage::ecs {

struct QueryParamsTag {};

template <typename T>
concept IsQueryParams = std::derived_from<T, QueryParamsTag>;

template <typename... Ts>
concept IsQueryParamsList = ((IsQueryParams<Ts>) && ...);

// --- Ref ---

struct QueryParamsTag_Ref : QueryParamsTag {};

template <typename... Ts>
  requires IsComponentRefList<Ts...>
struct Ref : QueryParamsTag_Ref {
  using TypeList = std::tuple<Ts...>;
};

// --- With ---

struct QueryParamsTag_With : QueryParamsTag {};

template <typename... Ts>
  requires IsComponentList<Ts...>
struct With : QueryParamsTag_With {
  using TypeList = std::tuple<Ts...>;
};

// --- Without ---

struct QueryParamsTag_Without : QueryParamsTag {};

template <typename... Ts>
  requires IsComponentList<Ts...>
struct Without : QueryParamsTag_Without {
  using TypeList = std::tuple<Ts...>;
};

// ----------

template <typename ParamsTag, typename T, typename... Ts>
  requires IsQueryParams<ParamsTag> && IsQueryParams<T>
struct QueryParamsTypeList {
  // clang-format off
  using TypeList = std::conditional_t<std::derived_from<T, ParamsTag>,
    typename T::TypeList,
    typename QueryParamsTypeList<ParamsTag, Ts...>::TypeList
  >;
  // clang-format on
};

template <typename ParamsTag, typename T>
  requires IsQueryParams<ParamsTag> && IsQueryParams<T>
struct QueryParamsTypeList<ParamsTag, T> {
  // clang-format off
  using TypeList = std::conditional_t<std::derived_from<T, ParamsTag>,
    typename T::TypeList,
    std::tuple<>
  >;
  // clang-format on
};

template <typename... Ts>
  requires IsQueryParamsList<Ts...>
class Query {
 public:
  using RefTypeList =
      typename QueryParamsTypeList<QueryParamsTag_Ref, Ts...>::TypeList;
  using WithTypeList =
      typename QueryParamsTypeList<QueryParamsTag_With, Ts...>::TypeList;
  using WithoutTypeList =
      typename QueryParamsTypeList<QueryParamsTag_Without, Ts...>::TypeList;

 private:  // TODO
};

template <typename... Ts>
struct Extract<Query<Ts...>> {
  static Query<Ts...> From([[maybe_unused]] Context& context) {
    // TODO
    return Query<Ts...>();
  }
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SYSTEM_QUERY
