#include "mirage_ecs/archetype/type_id.hpp"

using namespace mirage::ecs;

bool TypeMeta::operator==(const TypeMeta& other) const {
  return hash_code_ == other.hash_code_ && type_index_ == other.type_index_;
}

std::strong_ordering TypeMeta::operator<=>(const TypeMeta& other) const {
  return type_index_ <=> other.type_index_;
}

const char* TypeMeta::GetTypeName() const { return type_index_.name(); }

size_t TypeMeta::GetTypeSize() const { return type_size_; }

size_t TypeMeta::GetHashCode() const { return hash_code_; }

size_t TypeMeta::GetBitFlag() const { return bit_flag_; }

TypeMeta::TypeMeta(std::type_index type_index, size_t type_size)
    : type_index_(type_index),
      type_size_(type_size),
      hash_code_(type_index.hash_code()),
      bit_flag_(static_cast<size_t>(1) << hash_code_ % 64) {}

TypeId::TypeId(const TypeMeta& type_meta) : type_meta_(&type_meta) {}

bool TypeId::operator==(const TypeId& other) const {
  return *type_meta_ == *other.type_meta_;
}

std::strong_ordering TypeId::operator<=>(const TypeId& other) const {
  return *type_meta_ <=> *other.type_meta_;
}

const char* TypeId::GetTypeName() const { return type_meta_->GetTypeName(); }

size_t TypeId::GetTypeSize() const { return type_meta_->GetTypeSize(); }

size_t TypeId::GetHashCode() const { return type_meta_->GetHashCode(); }

size_t TypeId::GetBitFlag() const { return type_meta_->GetBitFlag(); }
