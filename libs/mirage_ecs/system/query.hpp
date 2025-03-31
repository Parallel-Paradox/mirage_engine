#ifndef MIRAGE_ECS_SYSTEM_QUERY
#define MIRAGE_ECS_SYSTEM_QUERY

#include <tuple>

#include "mirage_ecs/marker.hpp"
#include "mirage_ecs/system/extract.hpp"

namespace mirage::ecs {

struct QueryParamsTag {};

template <typename T>
concept IsQueryParams = std::derived_from<T, QueryParamsTag>;

template <typename... Args>
concept IsQueryParamsList = ((IsQueryParams<Args>) && ...);

// --- Ref ---

struct QueryParamsTag_Ref : QueryParamsTag {};

template <typename... Args>
  requires IsComponentRefList<Args...>
struct Ref : QueryParamsTag_Ref {
  using TypeList = std::tuple<Args...>;
};

// --- With ---

struct QueryParamsTag_With : QueryParamsTag {};

template <typename... Args>
  requires IsComponentList<Args...>
struct With : QueryParamsTag_With {
  using TypeList = std::tuple<Args...>;
};

// --- Without ---

struct QueryParamsTag_Without : QueryParamsTag {};

template <typename... Args>
  requires IsComponentList<Args...>
struct Without : QueryParamsTag_Without {
  using TypeList = std::tuple<Args...>;
};

// ----------

template <typename... Arg>
  requires IsQueryParamsList<Arg...>
class Query {
 public:
 private:
};

template <typename... Args>
struct Extract<Query<Args...>> {
  static Query<Args...> From([[maybe_unused]] Context& context) {
    return Query<Args...>();
  }
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SYSTEM_QUERY
