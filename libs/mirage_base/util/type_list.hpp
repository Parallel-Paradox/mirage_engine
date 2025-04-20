#ifndef MIRAGE_BASE_UTIL_TYPE_LIST
#define MIRAGE_BASE_UTIL_TYPE_LIST

#include <cstddef>

namespace mirage::base {

template <size_t I, typename Head, typename... Tail>
  requires /* check bound */ (I < 1 + sizeof...(Tail))
struct GetTypeFromArgs : GetTypeFromArgs<I - 1, Tail...> {};

template <typename Head, typename... Tail>
struct GetTypeFromArgs<0, Head, Tail...> {
  using Type = Head;
};

template <typename... Ts>
struct TypeList {
  [[nodiscard]] consteval static size_t size() noexcept {
    return sizeof...(Ts);
  }

  template <size_t I>
    requires /* check bound */ (I < size())
  struct Get {
    using Type = typename GetTypeFromArgs<I, Ts...>::Type;
  };
};

template <typename TypeList, size_t Index>
using GetTypeFromList = typename TypeList::template Get<Index>::Type;

}  // namespace mirage::base

#endif  // MIRAGE_BASE_UTIL_TYPE_LIST
