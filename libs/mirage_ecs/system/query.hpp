#ifndef MIRAGE_ECS_SYSTEM_QUERY
#define MIRAGE_ECS_SYSTEM_QUERY

#include "mirage_base/util/type_list.hpp"
#include "mirage_ecs/system/extract.hpp"
#include "mirage_ecs/util/marker.hpp"

namespace mirage::ecs {

struct QueryParamsTag {};

template <typename... Ts>
concept IsQueryParam = ((std::derived_from<Ts, QueryParamsTag>) && ...);

// --- Ref ---

struct QueryParamsTag_Ref : QueryParamsTag {};

template <typename... Ts>
  requires IsComponentRef<Ts...>
struct Ref : QueryParamsTag_Ref {
  using TypeList = base::TypeList<Ts...>;
};

// --- With ---

struct QueryParamsTag_With : QueryParamsTag {};

template <typename... Ts>
  requires IsComponent<Ts...>
struct With : QueryParamsTag_With {
  using TypeList = base::TypeList<Ts...>;
};

// --- Without ---

struct QueryParamsTag_Without : QueryParamsTag {};

template <typename... Ts>
  requires IsComponent<Ts...>
struct Without : QueryParamsTag_Without {
  using TypeList = base::TypeList<Ts...>;
};

// TODO: --- Or ---

// TODO: --- And ---

// ----------

template <typename ParamsTag, typename T, typename... Ts>
  requires IsQueryParam<ParamsTag> && IsQueryParam<T>
struct QueryParamsTypeList {
  // clang-format off
  using TypeList = std::conditional_t<std::derived_from<T, ParamsTag>,
    typename T::TypeList,
    typename QueryParamsTypeList<ParamsTag, Ts...>::TypeList
  >;
  // clang-format on
};

template <typename ParamsTag, typename T>
  requires IsQueryParam<ParamsTag> && IsQueryParam<T>
struct QueryParamsTypeList<ParamsTag, T> {
  // clang-format off
  using TypeList = std::conditional_t<std::derived_from<T, ParamsTag>,
    typename T::TypeList,
    base::TypeList<>
  >;
  // clang-format on
};

template <typename... Ts>
  requires IsQueryParam<Ts...>
class Query {
 public:
  using RefTypeList =
      typename QueryParamsTypeList<QueryParamsTag_Ref, Ts...>::TypeList;
  using WithTypeList =
      typename QueryParamsTypeList<QueryParamsTag_With, Ts...>::TypeList;
  using WithoutTypeList =
      typename QueryParamsTypeList<QueryParamsTag_Without, Ts...>::TypeList;

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
  static Query<Ts...> From([[maybe_unused]] Context& context) {
    // TODO
    return Query<Ts...>();
  }
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SYSTEM_QUERY
