#include "mirage_ecs/entity/entity_chunk.hpp"

#include <cstddef>
#include <new>
#include <utility>

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/define/check.hpp"
#include "mirage_ecs/entity/entity_layout.hpp"
#include "mirage_ecs/util/marker.hpp"

using namespace mirage;
using namespace mirage::ecs;

EntityChunk::~EntityChunk() { Clear(); }

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
  MIRAGE_DCHECK(raw_ptr_ != nullptr);
  MIRAGE_DCHECK(capacity_ > 0);
}

bool EntityChunk::Push(ComponentPackage &component_package) {
  MIRAGE_DCHECK(entity_layout_->component_type_set() ==
                component_package.type_set());
  if (size_ < capacity_) {
    return false;
  }
  std::byte *entity_ptr = raw_ptr_ + size_ * entity_layout_->size();
  for (const auto &kv : entity_layout_->component_meta_map()) {
    const auto &type_id = kv.key();
    const auto &component_meta = kv.val();

    auto component_data = component_package.Remove(type_id).Unwrap();
    std::byte *type_ptr = entity_ptr + component_meta.offset;
    component_meta.func_table.move(std::move(component_data).Take(),
                                   static_cast<void *>(type_ptr));
  }
  ++size_;
  return true;
}

bool EntityChunk::SwapRemove(const size_t index) {
  MIRAGE_DCHECK(index < size_);
  // TODO
  return true;
}

void EntityChunk::Clear() {
  if (raw_ptr_ == nullptr) {
    return;
  }

  for (size_t i = 0; i < size_; ++i) {
    std::byte *entity_ptr = raw_ptr_ + i * entity_layout_->size();
    for (const auto &component_meta : entity_layout_->component_meta_map()) {
      std::byte *type_ptr = entity_ptr + component_meta.val().offset;
      component_meta.val().func_table.destruct(static_cast<void *>(type_ptr));
    }
  }
  ::operator delete[](raw_ptr_, std::align_val_t{entity_layout_->align()});

  entity_layout_ = nullptr;
  byte_size_ = 0;
  raw_ptr_ = nullptr;
  capacity_ = 0;
  size_ = 0;
}

EntityView EntityChunk::operator[](const size_t index) {
  MIRAGE_DCHECK(index < size_);
  return EntityView(entity_layout_.raw_ptr(),
                    raw_ptr_ + index * entity_layout_->size());
}

const EntityView EntityChunk::operator[](const size_t index) const {
  MIRAGE_DCHECK(index < size_);
  return EntityView(entity_layout_.raw_ptr(),
                    raw_ptr_ + index * entity_layout_->size());
}

const EntityLayout &EntityChunk::entity_layout() const {
  return *entity_layout_;
}

size_t EntityChunk::byte_size() const { return byte_size_; }

std::byte *EntityChunk::raw_ptr() const { return raw_ptr_; }

size_t EntityChunk::capacity() const { return capacity_; }

size_t EntityChunk::size() const { return size_; }

EntityView::EntityView(EntityLayout *entity_layout, std::byte *raw_ptr)
    : entity_layout_(entity_layout), raw_ptr_(raw_ptr) {}
