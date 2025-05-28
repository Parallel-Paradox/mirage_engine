#ifndef MIRAGE_ECS_ENTITY_ENTITY_MANAGER
#define MIRAGE_ECS_ENTITY_ENTITY_MANAGER

#include <cstddef>

#include "mirage_base/container/array.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/archetype.hpp"

namespace mirage::ecs {

class MIRAGE_ECS EntityId {
 public:
  EntityId() = default;
  ~EntityId() = default;

  EntityId(const EntityId &) = default;
  EntityId &operator=(const EntityId &) = default;

  EntityId(size_t index, size_t generation);

  bool operator==(const EntityId &other) const;

  size_t index() const;
  size_t generation() const;

 private:
  size_t index_{0};
  size_t generation_{0};
};

class EntityManager {
  template <typename T>
  using Array = base::Array<T>;

 public:
  MIRAGE_ECS EntityManager() = default;
  MIRAGE_ECS ~EntityManager() = default;

  EntityManager(const EntityManager &) = delete;
  EntityManager &operator=(const EntityManager &) = delete;

  MIRAGE_ECS EntityManager(EntityManager &&other) noexcept = default;
  MIRAGE_ECS EntityManager &operator=(EntityManager &&other) noexcept = default;

  EntityId Create();
  void Destroy(EntityId entity_id);

 private:
  Array<EntityId> available_id_array_;

  Array<Archetype> archetype_array_;
  struct Route {
    size_t archetype_index{0};
    size_t entity_index{0};
  };
  Array<Route> index_route_map_;
  // route = index_route_map_[entity_id.index()]
  // iter = archetype_array_[route.archetype_index].begin() + route.entity_index
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ENTITY_MANAGER
