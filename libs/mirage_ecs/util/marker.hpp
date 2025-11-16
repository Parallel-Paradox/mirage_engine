#ifndef MIRAGE_ECS_UTIL_MARKER
#define MIRAGE_ECS_UTIL_MARKER

#include <concepts>

#include "mirage_base/wrap/box.hpp"

#define MIRAGE_COMPONENT static constexpr bool mirage_ecs_is_component = true
#define MIRAGE_RESOURCE static constexpr bool mirage_ecs_is_resource = true

namespace mirage::ecs {

template <typename T>
concept IsComponent = T::mirage_ecs_is_component && std::move_constructible<T>;

template <typename T>
concept IsComponentRef =
    std::is_reference_v<T> && IsComponent<std::remove_reference_t<T>>;

template <typename T>
struct ComponentConstraint : std::bool_constant<IsComponent<T>> {};

using BoxComponent = base::Box<ComponentConstraint>;

template <typename T>
concept IsResource = T::mirage_ecs_is_resource && std::move_constructible<T>;

template <typename T>
struct ResourceConstraint : std::bool_constant<IsResource<T>> {};

using BoxResource = base::Box<ResourceConstraint>;

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_UTIL_MARKER
