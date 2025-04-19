#ifndef MIRAGE_ECS_SYSTEM_EXTRACT
#define MIRAGE_ECS_SYSTEM_EXTRACT

#include <concepts>

#include "mirage_ecs/system/context.hpp"

namespace mirage::ecs {

template <typename T>
struct Extract;

template <typename T>
concept IsExtractType = requires(Context& context) {
  { Extract<T>::From(context) } -> std::same_as<T>;
};

template <typename... Ts>
concept ExtractTypeList = (IsExtractType<Ts> && ...);

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SYSTEM_EXTRACT
