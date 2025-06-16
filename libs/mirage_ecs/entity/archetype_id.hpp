#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE_ID
#define MIRAGE_ECS_ENTITY_ARCHETYPE_ID

#include <cstddef>

#include "mirage_ecs/define/export.hpp"

namespace mirage::ecs {

class MIRAGE_ECS ArchetypeId {
 public:
  ArchetypeId() = default;
  ArchetypeId(size_t index, size_t generation)
      : index_(index), generation_(generation) {}
  ~ArchetypeId() = default;

  ArchetypeId(const ArchetypeId &) = default;
  ArchetypeId &operator=(const ArchetypeId &) = default;

  bool operator==(const ArchetypeId &other) const {
    return index_ == other.index_ && generation_ == other.generation_;
  }

  size_t index() const { return index_; }
  size_t generation() const { return generation_; }

 private:
  size_t index_{0};
  size_t generation_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE_ID
