#ifndef MIRAGE_ECS_MARKER
#define MIRAGE_ECS_MARKER

#include <concepts>

namespace mirage::ecs {

struct Component {};

template <typename T>
concept IsComponent = std::derived_from<T, Component>;

template <typename... Args>
concept IsComponentList = ((IsComponent<Args>) && ...);

template <typename... Args>
concept IsComponentRefList = ((std::is_reference_v<Args> &&
                               IsComponent<std::remove_reference_t<Args>>) &&
                              ...);

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_MARKER
