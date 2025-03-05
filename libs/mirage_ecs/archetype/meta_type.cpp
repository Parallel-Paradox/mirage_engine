#include "mirage_ecs/archetype/meta_type.hpp"

using namespace mirage::ecs;

size_t MetaType::GetTypeId() const { return type_id_; }

MetaType::MetaType(const size_t type_id) : type_id_(type_id) {}
