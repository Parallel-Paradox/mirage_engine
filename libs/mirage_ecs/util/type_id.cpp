#include "mirage_ecs/util/type_id.hpp"

using namespace mirage::ecs;

bool TypeMeta::operator==(const TypeMeta& other) const {
  return (this == &other) ||
         (hash_code_ == other.hash_code_ && type_index_ == other.type_index_);
}

bool TypeMeta::operator!=(const TypeMeta& other) const {
  return !this->operator==(other);
}

std::strong_ordering TypeMeta::operator<=>(const TypeMeta& other) const {
  if (this->operator==(other)) {
    return std::strong_ordering::equal;
  }
  if (type_index_ < other.type_index_) {
    return std::strong_ordering::less;
  }
  return std::strong_ordering::greater;
}

const char* TypeMeta::type_name() const { return type_index_.name(); }

size_t TypeMeta::type_size() const { return type_size_; }

size_t TypeMeta::type_align() const { return type_align_; }

size_t TypeMeta::hash_code() const { return hash_code_; }

size_t TypeMeta::bit_flag() const { return bit_flag_; }

TypeMeta::TypeMeta(const std::type_index type_index, const size_t type_size,
                   const size_t type_align)
    : type_index_(type_index),
      type_size_(type_size),
      type_align_(type_align),
      hash_code_(type_index.hash_code()),
      bit_flag_(static_cast<size_t>(1) << hash_code_ % (sizeof(size_t) * 8)) {}

TypeId::TypeId(const TypeMeta& type_meta) : type_meta_(&type_meta) {}

bool TypeId::operator==(const TypeId& other) const {
  return *type_meta_ == *other.type_meta_;
}

bool TypeId::operator!=(const TypeId& other) const {
  return !this->operator==(other);
}

std::strong_ordering TypeId::operator<=>(const TypeId& other) const {
  return *type_meta_ <=> *other.type_meta_;
}

const char* TypeId::type_name() const { return type_meta_->type_name(); }

size_t TypeId::type_size() const { return type_meta_->type_size(); }

size_t TypeId::type_align() const { return type_meta_->type_align(); }

size_t TypeId::hash_code() const { return type_meta_->hash_code(); }

size_t TypeId::bit_flag() const { return type_meta_->bit_flag(); }
