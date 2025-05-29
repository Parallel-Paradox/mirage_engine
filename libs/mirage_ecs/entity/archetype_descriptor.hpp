#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE_DESCRIPTOR
#define MIRAGE_ECS_ENTITY_ARCHETYPE_DESCRIPTOR

#include <concepts>

#include "mirage_base/container/hash_map.hpp"
#include "mirage_ecs/component/component_id.hpp"
#include "mirage_ecs/util/marker.hpp"
#include "mirage_ecs/util/type_set.hpp"

namespace mirage::ecs {

class ArchetypeDescriptor {
  using TypeId = base::TypeId;

 public:
  using OffsetMap = base::HashMap<ComponentId, size_t>;

  MIRAGE_ECS ArchetypeDescriptor(base::Array<ComponentId> component_id_array);
  MIRAGE_ECS ~ArchetypeDescriptor() = default;

  ArchetypeDescriptor(const ArchetypeDescriptor &) = delete;
  ArchetypeDescriptor &operator=(const ArchetypeDescriptor &) = delete;

  MIRAGE_ECS ArchetypeDescriptor(ArchetypeDescriptor &&) = default;
  MIRAGE_ECS ArchetypeDescriptor &operator=(ArchetypeDescriptor &&) = default;

  template <IsComponent... Ts>
  static ArchetypeDescriptor New();

  [[nodiscard]] MIRAGE_ECS size_t align() const;
  [[nodiscard]] MIRAGE_ECS size_t size() const;
  [[nodiscard]] MIRAGE_ECS const OffsetMap &offset_map() const;
  [[nodiscard]] MIRAGE_ECS const TypeSet &type_set() const;

 private:
  size_t align_{0};
  size_t size_{0};
  OffsetMap offset_map_{};
  TypeSet type_set_{};
};

template <IsComponent... Ts>
ArchetypeDescriptor ArchetypeDescriptor::New() {
  base::Array<ComponentId> component_id_array;
  component_id_array.Reserve(sizeof...(Ts));
  (component_id_array.Push(ComponentId::Of<Ts>()), ...);
  return ArchetypeDescriptor(std::move(component_id_array));
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE_DESCRIPTOR
