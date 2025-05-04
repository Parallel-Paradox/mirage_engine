#include "mirage_ecs/entity/entity_chunk.hpp"

#include <cstddef>
#include <new>
#include <utility>

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/define.hpp"
#include "mirage_ecs/entity/entity_layout.hpp"
#include "mirage_ecs/util/marker.hpp"

using namespace mirage;
using namespace mirage::ecs;

EntityChunk::~EntityChunk() {
  if (raw_ptr_ == nullptr) {
    return;
  }

  for (size_t i = 0; i < size_; ++i) {
    std::byte *entity_ptr = raw_ptr_ + i * entity_layout_->size();
    for (const auto &component_meta : entity_layout_->component_meta_map()) {
      std::byte *type_ptr = entity_ptr + component_meta.val().offset;
      component_meta.val().destructor(static_cast<void *>(type_ptr));
    }
  }
  ::operator delete[](raw_ptr_, std::align_val_t{entity_layout_->align()});

  byte_size_ = 0;
  capacity_ = 0;
  size_ = 0;
}

EntityChunk::EntityChunk(EntityChunk &&other) noexcept
    : entity_layout_(std::move(other.entity_layout_)),
      byte_size_(other.byte_size_),
      raw_ptr_(other.raw_ptr_),
      capacity_(other.capacity_),
      size_(other.size_) {
  other.entity_layout_ = nullptr;
  other.byte_size_ = 0;
  other.raw_ptr_ = nullptr;
  other.capacity_ = 0;
  other.size_ = 0;
}

EntityChunk &EntityChunk::operator=(EntityChunk &&other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~EntityChunk();
  new (this) EntityChunk(std::move(other));
  return *this;
}

EntityChunk::EntityChunk(base::SharedLocal<EntityLayout> &&entity_layout,
                         const size_t capacity)
    : entity_layout_(std::move(entity_layout)),
      byte_size_(entity_layout->size() * capacity),
      raw_ptr_(static_cast<std::byte *>(::operator new[](
          byte_size_, std::align_val_t{entity_layout_->align()}))),
      capacity_(capacity) {
  MIRAGE_DCHECK(entity_layout_ != nullptr);
}

const EntityLayout &EntityChunk::entity_layout() const {
  return *entity_layout_;
}

size_t EntityChunk::byte_size() const { return byte_size_; }

std::byte *EntityChunk::raw_ptr() const { return raw_ptr_; }

size_t EntityChunk::capacity() const { return capacity_; }

size_t EntityChunk::size() const { return size_; }
