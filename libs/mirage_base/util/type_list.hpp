#ifndef MIRAGE_BASE_UTIL_TYPE_LIST
#define MIRAGE_BASE_UTIL_TYPE_LIST

#include <cstddef>

namespace mirage::base {

template <typename T>
struct ArgsWrapper {
  using Type = T;
};

template <size_t I, typename Head, typename... Tail>
  requires /* check bound */ (I <= sizeof...(Tail))
consteval auto GetFromArgs() {
  if constexpr (I == 0) {
    return ArgsWrapper<Head>();
  } else {
    return GetFromArgs<I - 1, Tail...>();
  }
}

template <size_t I, typename... Args>
  requires /* check bound */ (I < sizeof...(Args))
using GetTypeFromArgs = typename decltype(GetFromArgs<I, Args...>())::Type;

template <typename... Ts>
struct TypeList {
  [[nodiscard]] consteval static size_t size() noexcept {
    return sizeof...(Ts);
  }

  template <size_t I>
    requires /* check bound */ (I < size())
  struct Get {
    using Type = GetTypeFromArgs<I, Ts...>;
  };
};

template <typename TypeList, size_t Index>
using GetTypeFromList = typename TypeList::template Get<Index>::Type;

}  // namespace mirage::base

#endif  // MIRAGE_BASE_UTIL_TYPE_LIST
