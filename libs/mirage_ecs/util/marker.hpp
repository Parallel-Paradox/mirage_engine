#ifndef MIRAGE_ECS_UTIL_MARKER
#define MIRAGE_ECS_UTIL_MARKER

#include <concepts>

#include "mirage_base/util/type_list.hpp"

namespace mirage::ecs {

struct Component {};

template <typename... Ts>
concept IsComponent =
    ((std::derived_from<Ts, Component> && std::move_constructible<Ts>) && ...);

template <typename... Ts>
concept IsComponentRef =
    ((std::is_reference_v<Ts> && IsComponent<std::remove_reference_t<Ts>>) &&
     ...);

struct Resource {};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_UTIL_MARKER
