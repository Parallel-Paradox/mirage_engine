#ifndef MIRAGE_FRAMEWORK_BASE_CONTAINER_CONCEPT
#define MIRAGE_FRAMEWORK_BASE_CONTAINER_CONCEPT

#include <concepts>

namespace mirage {

template <typename T>
concept BasicValueType = std::default_initializable<T> && std::movable<T>;

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_CONTAINER_CONCEPT
