#include "mirage_ecs/entity/archetype.hpp"

using namespace mirage::ecs;

Archetype::Archetype(const SharedDescriptor& descriptor,
                     PagePoolObserver&& page)
    : descriptor_(descriptor.Clone()), page_pool_(std::move(page)) {}

void Archetype::Push(EntityId id, ComponentBundle& bundle) {
  // TODO
}

void Archetype::Push(Courier&& courier) {
  // TODO
}
