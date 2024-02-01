#include "mirage_framework/base/util/meta_type.hpp"

using namespace mirage;

bool MetaType::operator==(const MetaType& rhs) const {
  return type_id_ == rhs.type_id_;
}

bool MetaType::operator!=(const MetaType& rhs) const {
  return type_id_ != rhs.type_id_;
}

const char* MetaType::GetName() const {
  return name_;
}

MetaType::TypeId MetaType::GetTypeId() const {
  return type_id_;
}

MetaType::MetaType(const char* name, TypeId type_id)
    : name_(name), type_id_(type_id) {}
