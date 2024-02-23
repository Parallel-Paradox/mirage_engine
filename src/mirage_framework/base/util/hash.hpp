#ifndef MIRAGE_FRAMEWORK_BASE_UTIL_HASH
#define MIRAGE_FRAMEWORK_BASE_UTIL_HASH

#include <concepts>
#include <cstddef>

#include "mirage_framework/define.hpp"

namespace mirage {

template <typename T>
struct Hash {};

template <>
struct MIRAGE_API Hash<size_t> {
  size_t operator()(size_t val) const { return val; }
};

template <typename T>
concept HashKeyType = requires(Hash<T> hasher, T val) {
  { hasher(val) } -> std::same_as<size_t>;
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_UTIL_HASH
