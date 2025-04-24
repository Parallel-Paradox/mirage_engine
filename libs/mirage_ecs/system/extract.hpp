#ifndef MIRAGE_ECS_SYSTEM_EXTRACT
#define MIRAGE_ECS_SYSTEM_EXTRACT

#include <concepts>

namespace mirage::ecs {

class World;
class SystemContext;

template <typename T>
struct Extract;

template <typename T>
concept IsExtractable = requires(World& world, SystemContext& context) {
  { Extract<T>::From(world, context) } -> std::same_as<T>;
};

template <typename... Ts>
concept IsExtractableList = (IsExtractable<Ts> && ...);

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SYSTEM_EXTRACT
