#include "mirage_ecs/archetype/archetype.hpp"

#include <spdlog/spdlog.h>

using namespace mirage::ecs;

Archetype::Archetype(Descriptor&& descriptor)
    : descriptor_(std::move(descriptor)) {
  // TODO: Initialize offset map
  // Sort type array by alignment then size from big to small
  // Save the padding of each type, if a padding is big enough for the smallest
  // type, find the most fit type to fill the padding. Change the sequence.
  // The alignment of the first type should be considered when calculate size.
}

const Archetype::Descriptor& Archetype::GetDescriptor() const {
  return descriptor_;
}
