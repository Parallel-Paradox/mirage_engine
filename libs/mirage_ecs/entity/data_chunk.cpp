#include "mirage_ecs/entity/data_chunk.hpp"

#include <cstddef>
#include <new>
#include <utility>

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/define.hpp"
#include "mirage_ecs/util/marker.hpp"

using namespace mirage;
using namespace mirage::ecs;

DataChunk::~DataChunk() {
  if (raw_ptr_ == nullptr) {
    return;
  }

  for (size_t i = 0; i < size_; ++i) {
    std::byte *entity_ptr = raw_ptr_ + i * header_->entity_size;
    for (const auto &type_addr_offset : header_->type_addr_offset_map) {
      std::byte *type_ptr = entity_ptr + type_addr_offset.val();
      ((Component *)type_ptr)->~Component();
    }
  }
  ::operator delete[](raw_ptr_, std::align_val_t(header_->entity_align));

  byte_size_ = 0;
  capacity_ = 0;
  size_ = 0;
}

DataChunk::DataChunk(DataChunk &&other) noexcept
    : header_(std::move(other.header_)),
      byte_size_(other.byte_size_),
      raw_ptr_(other.raw_ptr_),
      capacity_(other.capacity_),
      size_(other.size_) {
  other.header_ = nullptr;
  other.byte_size_ = 0;
  other.raw_ptr_ = nullptr;
  other.capacity_ = 0;
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

DataChunk::DataChunk(base::SharedLocal<Header> &&header, size_t capacity)
    : header_(std::move(header)),
      byte_size_(header_->entity_size * capacity),
      raw_ptr_(new(std::align_val_t(header_->entity_align))
                   std::byte[byte_size_]),
      capacity_(capacity),
      size_(0) {
  MIRAGE_DCHECK(header_ != nullptr);
}

const base::SharedLocal<DataChunk::Header> &DataChunk::header() const {
  return header_;
}

size_t DataChunk::byte_size() const { return byte_size_; }

std::byte *DataChunk::raw_ptr() const { return raw_ptr_; }

size_t DataChunk::capacity() const { return capacity_; }

size_t DataChunk::size() const { return size_; }
