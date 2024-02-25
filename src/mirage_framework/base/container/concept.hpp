#ifndef MIRAGE_FRAMEWORK_BASE_CONTAINER_CONCEPT
#define MIRAGE_FRAMEWORK_BASE_CONTAINER_CONCEPT

#include <concepts>
#include "mirage_framework/base/util/hash.hpp"

namespace mirage {

template <typename T>
concept BasicValueType =
    std::default_initializable<T> && std::move_constructible<T>;

template <typename T>
concept HashKeyType = BasicValueType<T> && std::equality_comparable<T> &&
                      requires(Hash<T> hasher, T val) {
  { hasher(val) } -> std::same_as<size_t>;
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_CONTAINER_CONCEPT
