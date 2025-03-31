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
  requires IsQueryParams<ParamsTag> && IsQueryParams<T> &&
           IsQueryParamsList<Ts...>
struct QueryParamsTypeList {
  using TypeList = std::conditional_t<
      std::derived_from<T, ParamsTag>, typename T::TypeList,
      std::conditional_t<sizeof...(Ts) != 0,
                         QueryParamsTypeList<ParamsTag, Ts...>, std::tuple<>>>;
};

template <typename... Ts>
  requires IsQueryParamsList<Ts...>
class Query {
 public:
  using RefTypeList = QueryParamsTypeList<QueryParamsTag_Ref, Ts...>;
  using WithTypeList = QueryParamsTypeList<QueryParamsTag_With, Ts...>;
  using WithoutTypeList = QueryParamsTypeList<QueryParamsTag_Without, Ts...>;

 private:
};

template <typename... Ts>
struct Extract<Query<Ts...>> {
  static Query<Ts...> From([[maybe_unused]] Context& context) {
    return Query<Ts...>();
  }
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SYSTEM_QUERY
