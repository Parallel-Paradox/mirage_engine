#ifndef MIRAGE_ECS_UTIL_MARKER
#define MIRAGE_ECS_UTIL_MARKER

#include <concepts>

namespace mirage::ecs {

struct Component {};

template <typename T>
concept IsComponent =
    std::derived_from<T, Component> && std::move_constructible<T>;

template <typename T>
concept IsComponentRef =
    std::is_reference_v<T> && IsComponent<std::remove_reference_t<T>>;

struct Resource {
  virtual ~Resource() {}
};

template <typename T>
concept IsResource =
    std::derived_from<T, Resource> && std::move_constructible<T>;

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_UTIL_MARKER
