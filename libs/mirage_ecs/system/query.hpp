#ifndef MIRAGE_ECS_SYSTEM_QUERY
#define MIRAGE_ECS_SYSTEM_QUERY

#include "mirage_ecs/system/extract.hpp"

namespace mirage::ecs {

template <typename... Args>
struct Ref {
  constexpr static bool IsValid() {
    return true;  // TODO
  }
};

template <typename... Args>
struct With {
  constexpr static bool IsValid() {
    return true;  // TODO
  }
};

template <typename... Args>
struct Without {
  constexpr static bool IsValid() {
    return true;  // TODO
  }
};

template <typename... Arg>
class Query {
 public:
  constexpr static bool IsValid() {
    return true;  // TODO
  }
};

template <typename... Args>
struct Extract<Query<Args...>> {
  static Query<Args...> From([[maybe_unused]] Context& context) {
    static_assert(Query<Args...>::IsValid());
    return Query<Args...>();
  }
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SYSTEM_QUERY
