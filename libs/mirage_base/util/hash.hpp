#ifndef MIRAGE_BASE_UTIL_HASH
#define MIRAGE_BASE_UTIL_HASH

#include <concepts>

namespace mirage::base {

template <typename T>  // NOLINT: Unused empty type.
struct Hash {};

template <typename T>
concept HashType =
    std::equality_comparable<T> &&
    std::move_constructible<Hash<std::remove_const_t<T>>> &&
    std::copy_constructible<Hash<std::remove_const_t<T>>> &&
    std::default_initializable<Hash<std::remove_const_t<T>>> &&
    requires(const Hash<std::remove_const_t<T>> hasher, const T val) {
      { hasher(val) } -> std::same_as<size_t>;
    };

template <>
struct Hash<int32_t> {
  size_t operator()(const int32_t val) const { return val; }
};

template <>
struct Hash<size_t> {
  size_t operator()(const size_t val) const { return val; }
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_UTIL_HASH
