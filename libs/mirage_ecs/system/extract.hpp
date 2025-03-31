#ifndef MIRAGE_ECS_SYSTEM_EXTRACT
#define MIRAGE_ECS_SYSTEM_EXTRACT

#include <concepts>

#include "mirage_ecs/system/context.hpp"

namespace mirage::ecs {

template <typename T>  // NOLINT: Unused empty type.
struct Extract {};

template <typename T>
concept ExtractType = requires(Context& context) {
  { Extract<T>::From(context) } -> std::same_as<T>;
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SYSTEM_EXTRACT
