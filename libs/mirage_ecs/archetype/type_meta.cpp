#include "mirage_ecs/archetype/type_meta.hpp"

using namespace mirage::ecs;

size_t TypeMeta::GetTypeId() const { return type_id_; }

TypeMeta::TypeMeta(const size_t type_id, const size_t type_size)
    : type_id_(type_id), type_size_(type_size) {}

size_t TypeMeta::GetTypeSize() const { return type_size_; }
