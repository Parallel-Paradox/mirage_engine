#ifndef MIRAGE_ECS_ENTITY_ENTITY_ID
#define MIRAGE_ECS_ENTITY_ENTITY_ID

#include <cstddef>

#include "mirage_ecs/define/export.hpp"

namespace mirage::ecs {

class MIRAGE_ECS EntityId {
 public:
  EntityId() = default;
  EntityId(size_t index, size_t generation)
      : index_(index), generation_(generation) {}
  ~EntityId() = default;

  EntityId(const EntityId &) = default;
  EntityId &operator=(const EntityId &) = default;

  bool operator==(const EntityId &other) const {
    return index_ == other.index_ && generation_ == other.generation_;
  }

  size_t index() const { return index_; }
  size_t generation() const { return generation_; }

 private:
  size_t index_{0};
  size_t generation_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ENTITY_ID
