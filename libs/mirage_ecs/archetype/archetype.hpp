#ifndef MIRAGE_ECS_ARCHETYPE_ARCHETYPE
#define MIRAGE_ECS_ARCHETYPE_ARCHETYPE

#include "mirage_base/container/hash_map.hpp"
#include "mirage_ecs/archetype/archetype_descriptor.hpp"
#include "mirage_ecs/define.hpp"
#include "mirage_ecs/util/type_id.hpp"

namespace mirage::ecs {

class Archetype {
 public:
  using Descriptor = ArchetypeDescriptor;

  MIRAGE_ECS Archetype() = default;
  MIRAGE_ECS ~Archetype() = default;

  MIRAGE_ECS Archetype(const Archetype &) = delete;
  MIRAGE_ECS Archetype &operator=(const Archetype &) = delete;

  MIRAGE_ECS Archetype(Archetype &&other) noexcept = default;
  MIRAGE_ECS Archetype &operator=(Archetype &&other) noexcept = default;

  template <typename... Ts>
  static Archetype New();
  explicit MIRAGE_ECS Archetype(Descriptor &&descriptor);

  [[nodiscard]] MIRAGE_ECS const Descriptor &GetDescriptor() const;

 private:
  // TODO
  base::HashMap<TypeId, size_t> offset_map_{};
  Descriptor descriptor_;
};

template <typename... Ts>
Archetype Archetype::New() {
  return Archetype(Descriptor::New<Ts...>());
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ARCHETYPE_ARCHETYPE
