#include "mirage_ecs/archetype/archetype.hpp"

#include <spdlog/spdlog.h>

using namespace mirage;
using namespace mirage::ecs;

Archetype::Descriptor::Descriptor(Descriptor&& other) noexcept
    : type_array_(std::move(other.type_array_)), mask_(other.mask_) {
  other.type_array_.Clear();
  other.mask_ = 0;
}

Archetype::Descriptor& Archetype::Descriptor::operator=(
    Descriptor&& other) noexcept {
  if (this == &other) return *this;
  this->~Descriptor();
  new (this) Descriptor(std::move(other));
  return *this;
}

Archetype::Descriptor Archetype::Descriptor::Clone() const {
  Descriptor descriptor;
  descriptor.type_array_ = type_array_;
  descriptor.mask_ = mask_;
  return descriptor;
}

void Archetype::Descriptor::AddTypeId(const TypeId type_id) {
  auto iter = type_array_.begin();
  while (iter != type_array_.end()) {
    if (*iter == type_id) {
      return;
    }
    if (*iter > type_id) break;
    ++iter;
  }
  mask_ |= type_id.GetBitFlag();
  if (iter != type_array_.end()) {
    type_array_.Insert(iter, type_id);
  } else {
    type_array_.Emplace(type_id);
  }
}

const base::Array<TypeId>& Archetype::Descriptor::GetTypeArray() const {
  return type_array_;
}

size_t Archetype::Descriptor::GetMask() const { return mask_; }

bool Archetype::Descriptor::operator==(const Descriptor& other) const {
  return mask_ == other.mask_ && type_array_ == other.type_array_;
}

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

bool Archetype::Descriptor::With(const Descriptor& desc) const {
  const auto& desc_type_array = desc.GetTypeArray();
  if (desc_type_array.IsEmpty()) return true;
  if ((desc.GetMask() | GetMask()) != GetMask() ||
      desc_type_array.GetSize() > type_array_.GetSize())
    return false;

  auto desc_type_iter = desc_type_array.begin();
  for (const auto& type_id : type_array_) {
    const auto& desc_type_id = *desc_type_iter;
    if (type_id == desc_type_id) ++desc_type_iter;
    if (type_id > desc_type_id) return false;
  }
  return desc_type_iter == desc_type_array.end();
}

bool Archetype::Descriptor::Without(const Descriptor& desc) const {
  const auto& desc_type_array = desc.GetTypeArray();
  if (type_array_.IsEmpty() || desc_type_array.IsEmpty()) return true;

  auto desc_type_iter = desc_type_array.begin();
  for (const auto& type_id : type_array_) {
    while (type_id > *desc_type_iter) {
      ++desc_type_iter;
      if (desc_type_iter == desc_type_array.end()) return true;
    }
    if (type_id == *desc_type_iter) return false;
  }
  return true;
}
