#ifndef MIRAGE_ECS_SYSTEM_QUERY
#define MIRAGE_ECS_SYSTEM_QUERY

#include "mirage_base/auto_ptr/owned.hpp"
#include "mirage_base/util/type_list.hpp"
#include "mirage_ecs/system/extract.hpp"
#include "mirage_ecs/util/marker.hpp"

namespace mirage::ecs {

struct QueryParamsTag {};

template <typename... Ts>
concept IsQueryParam = ((std::derived_from<Ts, QueryParamsTag>) && ...);

// --- Ref ---

struct QueryParamsTag_Ref : QueryParamsTag {};

template <IsComponentRef... Ts>
struct Ref : QueryParamsTag_Ref {
  using TypeList = base::TypeList<Ts...>;
};

// --- With ---

struct QueryParamsTag_With : QueryParamsTag {};

template <IsComponent... Ts>
struct With : QueryParamsTag_With {
  using TypeList = base::TypeList<Ts...>;
};

// --- Without ---

struct QueryParamsTag_Without : QueryParamsTag {};

template <IsComponent... Ts>
struct Without : QueryParamsTag_Without {
  using TypeList = base::TypeList<Ts...>;
};

// TODO: --- Or ---

// TODO: --- And ---

// ----------

// TODO: Optimize finder
template <typename ParamsTag, typename T, typename... Ts>
  requires IsQueryParam<ParamsTag> && IsQueryParam<T> && IsQueryParam<Ts...>
consteval auto QueryParamsTypeList() {
  if constexpr (std::derived_from<T, ParamsTag>) {
    return typename T::TypeList();
  } else {
    return QueryParamsTypeList<ParamsTag, Ts...>();
  }
}

template <typename ParamsTag>
  requires IsQueryParam<ParamsTag>
consteval auto QueryParamsTypeList() {
  return base::TypeList();
}

template <typename... Ts>
  requires IsQueryParam<Ts...>
class Query {
 public:
  using RefTypeList =
      decltype(QueryParamsTypeList<QueryParamsTag_Ref, Ts...>());
  using WithTypeList =
      decltype(QueryParamsTypeList<QueryParamsTag_With, Ts...>());
  using WithoutTypeList =
      decltype(QueryParamsTypeList<QueryParamsTag_Without, Ts...>());

  class Iterator;
  class ConstIterator;

  Iterator begin();
  Iterator end();

  ConstIterator begin() const;
  ConstIterator end() const;

 private:  // TODO
};

template <typename... Ts>
struct Extract<Query<Ts...>> {
  static Query<Ts...> From(
      [[maybe_unused]] World& world,
      [[maybe_unused]] base::Owned<SystemContext>& context) {
    // TODO
    return Query<Ts...>();
  }
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SYSTEM_QUERY
