#include "mirage_ecs/util/type_set.hpp"

#include "mirage_base/util/type_id.hpp"

using namespace mirage;
using namespace mirage::ecs;

TypeSet::TypeSet(TypeSet&& other) noexcept
    : type_array_(std::move(other.type_array_)), mask_(other.mask_) {
  other.type_array_.Clear();
  other.mask_ = 0;
}

TypeSet& TypeSet::operator=(TypeSet&& other) noexcept {
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
    type_array_.Insert(iter - type_array_.begin(), type_id);
  } else {
    type_array_.Emplace(type_id);
  }
}

void TypeSet::RemoveTypeId(const TypeId& type_id) {
  auto remove_iter = type_array_.end();
  bool should_remove_bit_flag = true;
  for (auto iter = type_array_.begin(); iter != type_array_.end(); ++iter) {
    if (*iter > type_id && remove_iter == type_array_.end()) return;
    if (*iter == type_id) {
      remove_iter = iter;
    } else if (iter->bit_flag() == type_id.bit_flag()) {
      should_remove_bit_flag = false;
    }
  }
  if (remove_iter == type_array_.end()) {
    return;
  }
  type_array_.Remove(remove_iter - type_array_.begin());
  if (should_remove_bit_flag) {
    mask_ &= ~type_id.bit_flag();
  }
}

bool TypeSet::With(const TypeSet& set) const {
  const auto& set_type_array = set.type_array();
  if (set_type_array.empty()) return true;
  if ((set.mask() | mask_) != mask_ ||
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

bool TypeSet::With(const TypeId& type_id) const {
  if ((type_id.bit_flag() & mask_) == 0) return false;
  for (const auto& type : type_array_) {
    if (type == type_id) return true;
    if (type > type_id) return false;
  }
  return false;
}

bool TypeSet::Without(const TypeSet& set) const {
  const auto& set_type_array = set.type_array();
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

bool TypeSet::Without(const TypeId& type_id) const { return !With(type_id); }

const base::Array<base::TypeId>& TypeSet::type_array() const {
  return type_array_;
}

size_t TypeSet::mask() const { return mask_; }

bool TypeSet::operator==(const TypeSet& other) const {
  return mask_ == other.mask_ && type_array_ == other.type_array_;
}
