#ifndef MIRAGE_BASE_UTIL_TYPE_LIST
#define MIRAGE_BASE_UTIL_TYPE_LIST

#include <cstddef>

namespace mirage::base {

template <size_t I, typename Head, typename... Tail>
  requires(I < 1 + sizeof...(Tail))
struct GetType : GetType<I - 1, Tail...> {};

template <typename Head, typename... Tail>
struct GetType<0, Head, Tail...> {
  using Type = Head;
};

template <typename... Ts>
struct TypeList {
  [[nodiscard]] constexpr static size_t GetSize() noexcept {
    return sizeof...(Ts);
  }

  template <size_t I>
    requires(I < GetSize())
  struct Get {
    using Type = typename GetType<I, Ts...>::Type;
  };
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_UTIL_TYPE_LIST
