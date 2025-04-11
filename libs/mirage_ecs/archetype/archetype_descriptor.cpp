#include "mirage_ecs/archetype/archetype_descriptor.hpp"

using namespace mirage;
using namespace mirage::ecs;

ArchetypeDescriptor::ArchetypeDescriptor(ArchetypeDescriptor&& other) noexcept
    : type_array_(std::move(other.type_array_)), mask_(other.mask_) {
  other.type_array_.Clear();
  other.mask_ = 0;
}

ArchetypeDescriptor& ArchetypeDescriptor::operator=(
    ArchetypeDescriptor&& other) noexcept {
  if (this == &other) return *this;
  this->~ArchetypeDescriptor();
  new (this) ArchetypeDescriptor(std::move(other));
  return *this;
}

ArchetypeDescriptor ArchetypeDescriptor::Clone() const {
  ArchetypeDescriptor descriptor;
  descriptor.type_array_ = type_array_;
  descriptor.mask_ = mask_;
  return descriptor;
}

void ArchetypeDescriptor::AddTypeId(const TypeId type_id) {
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

bool ArchetypeDescriptor::With(const ArchetypeDescriptor& desc) const {
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

bool ArchetypeDescriptor::Without(const ArchetypeDescriptor& desc) const {
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

const base::Array<TypeId>& ArchetypeDescriptor::GetTypeArray() const {
  return type_array_;
}

size_t ArchetypeDescriptor::GetMask() const { return mask_; }

bool ArchetypeDescriptor::operator==(const ArchetypeDescriptor& other) const {
  return mask_ == other.mask_ && type_array_ == other.type_array_;
}
