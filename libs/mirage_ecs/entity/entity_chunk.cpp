#include "mirage_ecs/entity/entity_chunk.hpp"

#include <cstddef>
#include <new>
#include <utility>

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/define/check.hpp"
#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"
#include "mirage_ecs/util/marker.hpp"

using namespace mirage;
using namespace mirage::ecs;

EntityChunk::~EntityChunk() { Clear(); }

EntityChunk::EntityChunk(EntityChunk &&other) noexcept
    : archetype_descriptor_(std::move(other.archetype_descriptor_)),
      byte_size_(other.byte_size_),
      raw_ptr_(other.raw_ptr_),
      capacity_(other.capacity_),
      size_(other.size_) {
  other.archetype_descriptor_ = nullptr;
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

EntityChunk::EntityChunk(
    base::SharedLocal<ArchetypeDescriptor> &&archetype_descriptor,
    const size_t capacity)
    : archetype_descriptor_(std::move(archetype_descriptor)),
      byte_size_(archetype_descriptor->size() * capacity),
      raw_ptr_(static_cast<std::byte *>(::operator new[](
          byte_size_, std::align_val_t{archetype_descriptor_->align()}))),
      capacity_(capacity) {
  MIRAGE_DCHECK(archetype_descriptor_ != nullptr);
  MIRAGE_DCHECK(raw_ptr_ != nullptr);
  MIRAGE_DCHECK(capacity_ > 0);
}

bool EntityChunk::Push(ComponentBundle &component_bundle) {
  MIRAGE_DCHECK(archetype_descriptor_->type_set() ==
                component_bundle.MakeTypeSet());
  if (size_ < capacity_) {
    return false;
  }
  std::byte *entity_ptr = raw_ptr_ + size_ * archetype_descriptor_->size();
  for (const auto &kv : archetype_descriptor_->offset_map()) {
    ComponentId component_id = kv.key();
    size_t offset = kv.val();

    auto component_box =
        component_bundle.Remove(component_id.type_id()).Unwrap();
    std::byte *type_ptr = entity_ptr + offset;
    component_id.move_func()(std::move(component_box).raw_ptr(),
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
    std::byte *entity_ptr = raw_ptr_ + i * archetype_descriptor_->size();
    for (const auto &kv : archetype_descriptor_->offset_map()) {
      std::byte *type_ptr = entity_ptr + kv.val();
      kv.key().destruct_func()(static_cast<void *>(type_ptr));
    }
  }
  ::operator delete[](raw_ptr_,
                      std::align_val_t{archetype_descriptor_->align()});

  archetype_descriptor_ = nullptr;
  byte_size_ = 0;
  raw_ptr_ = nullptr;
  capacity_ = 0;
  size_ = 0;
}

EntityView EntityChunk::operator[](const size_t index) {
  MIRAGE_DCHECK(index < size_);
  return EntityView(archetype_descriptor_.raw_ptr(),
                    raw_ptr_ + index * archetype_descriptor_->size());
}

const EntityView EntityChunk::operator[](const size_t index) const {
  MIRAGE_DCHECK(index < size_);
  return EntityView(archetype_descriptor_.raw_ptr(),
                    raw_ptr_ + index * archetype_descriptor_->size());
}

const ArchetypeDescriptor &EntityChunk::archetype_descriptor() const {
  return *archetype_descriptor_;
}

size_t EntityChunk::byte_size() const { return byte_size_; }

std::byte *EntityChunk::raw_ptr() const { return raw_ptr_; }

size_t EntityChunk::capacity() const { return capacity_; }

size_t EntityChunk::size() const { return size_; }

EntityView::EntityView(ArchetypeDescriptor *archetype_descriptor,
                       std::byte *raw_ptr)
    : archetype_descriptor_(archetype_descriptor), raw_ptr_(raw_ptr) {}
