#ifndef MIRAGE_ECS_ENTITY_ENTITY_MANAGER
#define MIRAGE_ECS_ENTITY_ENTITY_MANAGER

#include <cstddef>

#include "mirage_base/auto_ptr/observed.hpp"
#include "mirage_base/container/array.hpp"
#include "mirage_base/container/hash_map.hpp"
#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/archetype.hpp"
#include "mirage_ecs/entity/archetype_id.hpp"
#include "mirage_ecs/entity/buffer/aligned_buffer_pool.hpp"
#include "mirage_ecs/entity/entity_id.hpp"
#include "mirage_ecs/util/type_set.hpp"

namespace mirage::ecs {

class EntityManager {
  using ObservedBufferPool = base::ObservedLocal<AlignedBufferPool>;

  template <typename T>
  using Array = base::Array<T>;

 public:
  class View;
  class ConstView;

  MIRAGE_ECS EntityManager() = default;
  MIRAGE_ECS ~EntityManager() = default;

  EntityManager(const EntityManager &) = delete;
  EntityManager &operator=(const EntityManager &) = delete;

  MIRAGE_ECS EntityManager(EntityManager &&other) noexcept = default;
  MIRAGE_ECS EntityManager &operator=(EntityManager &&other) noexcept = default;

  EntityId Create(ComponentBundle &bundle);
  void Destroy(const EntityId &entity_id);

  MIRAGE_ECS View Get(const EntityId &entity_id);
  MIRAGE_ECS ConstView Get(const EntityId &entity_id) const;

 private:
  ObservedBufferPool buffer_pool_;

  Array<ArchetypeId> available_archetype_id_;
  Array<Archetype> archetype_array_;
  base::HashMap<TypeSet, ArchetypeId> archetype_route_map_;

  struct Route {
    ArchetypeId archetype_id;
    size_t entity_index{0};
  };
  Array<EntityId> available_entity_id_;
  Array<Route> entity_route_array_;
};

class EntityManager::View {
 public:
  // TODO
 private:
  Archetype::View archetype_view_;
};

class EntityManager::ConstView {
 public:
  // TODO
 private:
  Archetype::ConstView archetype_view_;
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ENTITY_MANAGER
