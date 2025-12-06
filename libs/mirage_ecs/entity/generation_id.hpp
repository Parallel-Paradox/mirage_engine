#ifndef MIRAGE_ECS_ENTITY_GENERATION_ID
#define MIRAGE_ECS_ENTITY_GENERATION_ID

#include <cstddef>
#include <cstdint>

#include "mirage_ecs/define/export.hpp"

namespace mirage::ecs {

class MIRAGE_ECS GenerationId {
 public:
  constexpr static size_t kInvalidIndex = SIZE_MAX;
  constexpr static size_t kTombGeneration = SIZE_MAX;

  GenerationId() = default;
  GenerationId(const size_t index, const size_t generation)
      : index_(index), generation_(generation) {}
  ~GenerationId() { Reset(); }

  GenerationId(const GenerationId &) = default;
  GenerationId &operator=(const GenerationId &) = default;

  bool operator==(const GenerationId &other) const {
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

using EntityId = GenerationId;
using ArchetypeId = GenerationId;

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_GENERATION_ID
