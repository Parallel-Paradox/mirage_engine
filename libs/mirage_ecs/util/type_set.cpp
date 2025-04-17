#include "mirage_ecs/util/type_set.hpp"

using namespace mirage;
using namespace mirage::ecs;


TypeSet::TypeSet(TypeSet&& other) noexcept
    : type_array_(std::move(other.type_array_)), mask_(other.mask_) {
  other.type_array_.Clear();
  other.mask_ = 0;
}

TypeSet& TypeSet::operator=(
    TypeSet&& other) noexcept {
  if (this == &other) return *this;
  this->~TypeSet();
  new (this) TypeSet(std::move(other));
  return *this;
}

TypeSet TypeSet::Clone() const {
  TypeSet set;
  set.type_array_ = type_array_;
  set.mask_ = mask_;
  return set;
}

void TypeSet::AddTypeId(const TypeId type_id) {
  auto iter = type_array_.begin();
  while (iter != type_array_.end()) {
    if (*iter == type_id) {
      return;
    }
    if (*iter > type_id) break;
    ++iter;
  }
  mask_ |= type_id.bit_flag();
  if (iter != type_array_.end()) {
    type_array_.Insert(iter, type_id);
  } else {
    type_array_.Emplace(type_id);
  }
}

bool TypeSet::With(const TypeSet& set) const {
  const auto& set_type_array = set.GetTypeArray();
  if (set_type_array.empty()) return true;
  if ((set.GetMask() | GetMask()) != GetMask() ||
      set_type_array.size() > type_array_.size())
    return false;

  auto set_type_iter = set_type_array.begin();
  for (const auto& type_id : type_array_) {
    const auto& set_type_id = *set_type_iter;
    if (type_id == set_type_id) ++set_type_iter;
    if (type_id > set_type_id) return false;
  }
  return set_type_iter == set_type_array.end();
}

bool TypeSet::Without(const TypeSet& set) const {
  const auto& set_type_array = set.GetTypeArray();
  if (type_array_.empty() || set_type_array.empty()) return true;

  auto set_type_iter = set_type_array.begin();
  for (const auto& type_id : type_array_) {
    while (type_id > *set_type_iter) {
      ++set_type_iter;
      if (set_type_iter == set_type_array.end()) return true;
    }
    if (type_id == *set_type_iter) return false;
  }
  return true;
}

const base::Array<TypeId>& TypeSet::GetTypeArray() const {
  return type_array_;
}

size_t TypeSet::GetMask() const { return mask_; }

bool TypeSet::operator==(const TypeSet& other) const {
  return mask_ == other.mask_ && type_array_ == other.type_array_;
}
