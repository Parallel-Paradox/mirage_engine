#include "mirage_ecs/entity/data_chunk.hpp"

#include <new>
#include <utility>

using namespace mirage::ecs;

DataChunk::~DataChunk() {
  size_ = 0;
  if (raw_ptr_ != nullptr) {
    delete[] raw_ptr_;
  }
}

DataChunk::DataChunk(DataChunk &&other) noexcept {
  raw_ptr_ = other.raw_ptr_;
  size_ = other.size_;
  other.raw_ptr_ = nullptr;
  other.size_ = 0;
}

DataChunk &DataChunk::operator=(DataChunk &&other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~DataChunk();
  new (this) DataChunk(std::move(other));
  return *this;
}
