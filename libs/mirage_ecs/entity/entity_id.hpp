#ifndef MIRAGE_ECS_ENTITY_ENTITY_ID
#define MIRAGE_ECS_ENTITY_ENTITY_ID

#include <cstddef>
#include <cstdint>

#include "mirage_ecs/define/export.hpp"

namespace mirage::ecs {

class MIRAGE_ECS EntityId {
 public:
  constexpr static size_t kInvalidIndex = SIZE_MAX;
  constexpr static size_t kTombGeneration = SIZE_MAX;

  EntityId() = default;
  EntityId(size_t index, size_t generation)
      : index_(index), generation_(generation) {}
  ~EntityId() { Reset(); }

  EntityId(const EntityId &) = default;
  EntityId &operator=(const EntityId &) = default;

  bool operator==(const EntityId &other) const {
    return index_ == other.index_ && generation_ == other.generation_;
  }

  void Reset() {
    index_ = kInvalidIndex;
    generation_ = 0;
  }

  void MarkTomb() { generation_ = kTombGeneration; }

  [[nodiscard]] size_t index() const { return index_; }
  [[nodiscard]] size_t generation() const { return generation_; }

  [[nodiscard]] bool is_valid() const { return index_ != kInvalidIndex; }
  [[nodiscard]] bool is_tomb() const { return generation_ == kTombGeneration; }

 private:
  size_t index_{kInvalidIndex};
  size_t generation_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ENTITY_ID
