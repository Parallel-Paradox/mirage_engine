#include "mirage_ecs/entity/entity_manager.hpp"

using namespace mirage::ecs;

EntityId::EntityId(size_t index, size_t generation)
    : index_(index), generation_(generation) {}

size_t EntityId::index() const { return index_; }

size_t EntityId::generation() const { return generation_; }

bool EntityId::operator==(const EntityId &other) const {
  return index_ == other.index_ && generation_ == other.generation_;
}
