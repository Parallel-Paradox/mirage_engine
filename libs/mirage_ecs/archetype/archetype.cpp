#include "mirage_ecs/archetype/archetype.hpp"

using namespace mirage;
using namespace mirage::ecs;

const base::Array<const TypeMeta*>& Archetype::Descriptor::GetTypeArray()
    const {
  return type_array_;
}

size_t Archetype::Descriptor::GetHash() const { return hash_; }

bool Archetype::Descriptor::operator==(const Descriptor& other) const {
  return hash_ == other.hash_ && type_array_ == other.type_array_;
}
