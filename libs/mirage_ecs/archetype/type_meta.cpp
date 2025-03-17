#include "mirage_ecs/archetype/type_meta.hpp"

using namespace mirage::ecs;

TypeMeta::TypeMeta(const char* type_name, const size_t type_id,
                   const size_t type_size)
    : type_name_(type_name), type_id_(type_id), type_size_(type_size) {}

const char* TypeMeta::GetTypeName() const { return type_name_; }

size_t TypeMeta::GetTypeId() const { return type_id_; }

size_t TypeMeta::GetTypeSize() const { return type_size_; }
