#ifndef MIRAGE_ECS_UTIL_MARKER
#define MIRAGE_ECS_UTIL_MARKER

#include <concepts>

#include "mirage_base/util/type_list.hpp"

namespace mirage::ecs {

struct Component {};

template <typename T>
concept IsComponent =
    std::derived_from<T, Component> && std::move_constructible<T>;

template <typename... Ts>
concept IsComponentList = (IsComponent<Ts> && ...);

template <typename T>
concept IsComponentRef =
    std::is_reference_v<T> && IsComponent<std::remove_reference_t<T>>;

template <typename... Ts>
concept IsComponentRefList = (IsComponentRef<Ts> && ...);

struct Resource {};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_UTIL_MARKER
