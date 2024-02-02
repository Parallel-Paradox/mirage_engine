#include "mirage_framework/base/util/meta_type.hpp"

using namespace mirage;

const char* MetaType::GetName() const {
  return name_;
}

size_t MetaType::GetSize() const {
  return size_;
}

MetaType::MetaType(const char* name, size_t size) : name_(name), size_(size) {}
