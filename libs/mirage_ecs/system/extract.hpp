#ifndef MIRAGE_ECS_SYSTEM_EXTRACT
#define MIRAGE_ECS_SYSTEM_EXTRACT

#include <concepts>

#include "mirage_ecs/system/context.hpp"

namespace mirage::ecs {

template <typename T>
struct Extract;

template <typename T>
concept IsExtractable = requires(Context& context) {
  { Extract<T>::From(context) } -> std::same_as<T>;
};

template <typename... Ts>
concept IsExtractableList = (IsExtractable<Ts> && ...);

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SYSTEM_EXTRACT
