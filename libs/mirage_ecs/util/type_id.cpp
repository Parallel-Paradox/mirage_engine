#include "mirage_ecs/util/type_id.hpp"

using namespace mirage::ecs;

bool TypeMeta::operator==(const TypeMeta& other) const {
  return hash_code_ == other.hash_code_ && type_index_ == other.type_index_;
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

const char* TypeMeta::GetTypeName() const { return type_index_.name(); }

size_t TypeMeta::GetTypeSize() const { return type_size_; }

size_t TypeMeta::GetTypeAlign() const { return type_align_; }

size_t TypeMeta::GetHashCode() const { return hash_code_; }

size_t TypeMeta::GetBitFlag() const { return bit_flag_; }

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

const char* TypeId::GetTypeName() const { return type_meta_->GetTypeName(); }

size_t TypeId::GetTypeSize() const { return type_meta_->GetTypeSize(); }

size_t TypeId::GetTypeAlign() const { return type_meta_->GetTypeAlign(); }

size_t TypeId::GetHashCode() const { return type_meta_->GetHashCode(); }

size_t TypeId::GetBitFlag() const { return type_meta_->GetBitFlag(); }
