#include "mirage_ecs/entity/buffer/archetype_data_buffer.hpp"

#include <_types/_uint16_t.h>

#include "mirage_base/define/check.hpp"

using namespace mirage::ecs;

ArchetypeDataBuffer::ArchetypeDataBuffer(Buffer&& buffer,
                                         SharedDescriptor&& descriptor)
    : descriptor_(std::move(descriptor)), buffer_(std::move(buffer)) {
  MIRAGE_DCHECK(buffer_.align() >= alignof(EntityId));
  MIRAGE_DCHECK(buffer_.align() >= descriptor_->align());
  size_ = 0;
  capacity_ = static_cast<uint16_t>(buffer_.size()) /
              (descriptor_->size() + sizeof(EntityId));
}

ArchetypeDataBuffer::~ArchetypeDataBuffer() {
  Clear();
  descriptor_ = nullptr;
  capacity_ = 0;
}

ArchetypeDataBuffer::ArchetypeDataBuffer(ArchetypeDataBuffer&& other) noexcept
    : descriptor_(std::move(other.descriptor_)),
      buffer_(std::move(other.buffer_)),
      size_(other.size_),
      capacity_(other.capacity_) {
  other.size_ = 0;
  other.capacity_ = 0;
}

ArchetypeDataBuffer& ArchetypeDataBuffer::operator=(
    ArchetypeDataBuffer&& other) noexcept {
  if (this != &other) {
    this->~ArchetypeDataBuffer();
    new (this) ArchetypeDataBuffer(std::move(other));
  }
  return *this;
}

ArchetypeDataBuffer::ConstView ArchetypeDataBuffer::operator[](
    uint16_t index) const {
  MIRAGE_DCHECK(index < size_);
  const auto* view_ptr = buffer_.ptr() + index * descriptor_->size();
  const auto* entity_id_ptr =
      reinterpret_cast<const EntityId*>(buffer_.ptr() + buffer_.size()) -
      (capacity_ - index);
  return ConstView(descriptor_.raw_ptr(), view_ptr, entity_id_ptr);
}

ArchetypeDataBuffer::View ArchetypeDataBuffer::operator[](uint16_t index) {
  MIRAGE_DCHECK(index < size_);
  auto* view_ptr = buffer_.ptr() + index * descriptor_->size();
  auto* entity_id_ptr =
      reinterpret_cast<EntityId*>(buffer_.ptr() + buffer_.size()) -
      (capacity_ - index);
  return View(descriptor_.raw_ptr(), view_ptr, entity_id_ptr);
}

void ArchetypeDataBuffer::Push(const EntityId& id, ComponentBundle& bundle) {
  MIRAGE_DCHECK(size_ < capacity_);
  auto* view_ptr = buffer_.ptr() + size_ * descriptor_->size();
  for (const auto& entry : descriptor_->offset_map()) {
    const auto& component_id = entry.key();
    const auto& offset = entry.val();

    auto box_op = bundle.Remove(component_id.type_id());
    MIRAGE_DCHECK(box_op.is_valid());
    auto box = box_op.Unwrap();
    component_id.move_func()(box.raw_ptr(), view_ptr + offset);
  }

  auto* entity_id_ptr =
      reinterpret_cast<EntityId*>(buffer_.ptr() + buffer_.size()) -
      (capacity_ - size_);
  *entity_id_ptr = id;
}

void ArchetypeDataBuffer::Push(const EntityId& id, View&& view) {
  MIRAGE_DCHECK(size_ < capacity_);
  auto* view_ptr = buffer_.ptr() + size_ * descriptor_->size();
  for (const auto& entry : descriptor_->offset_map()) {
    const auto& component_id = entry.key();
    const auto& offset = entry.val();

    void* component_ptr = view.TryGet(component_id);
    MIRAGE_DCHECK(component_ptr != nullptr);
    component_id.move_func()(component_ptr, view_ptr + offset);
  }

  auto* entity_id_ptr =
      reinterpret_cast<EntityId*>(buffer_.ptr() + buffer_.size()) -
      (capacity_ - size_);
  *entity_id_ptr = id;
}

void ArchetypeDataBuffer::RemoveTail() {
  MIRAGE_DCHECK(size_ > 0);
  --size_;
  auto* entity_id_ptr =
      reinterpret_cast<EntityId*>(buffer_.ptr() + buffer_.size()) -
      (capacity_ - size_);
  entity_id_ptr->Reset();

  auto* view_ptr = buffer_.ptr() + size_ * descriptor_->size();
  for (const auto& entry : descriptor_->offset_map()) {
    const auto& component_id = entry.key();
    const auto& offset = entry.val();
    component_id.destruct_func()(view_ptr + offset);
  }
}

void ArchetypeDataBuffer::Clear() {
  while (size_ > 0) {
    RemoveTail();
  }
}

const ArchetypeDataBuffer::SharedDescriptor& ArchetypeDataBuffer::descriptor()
    const {
  return descriptor_;
}

uint16_t ArchetypeDataBuffer::size() const { return size_; }

uint16_t ArchetypeDataBuffer::capacity() const { return capacity_; }

ArchetypeDataBuffer::ConstView::ConstView(const ArchetypeDescriptor* descriptor,
                                          const std::byte* view_ptr,
                                          const EntityId* entity_id_ptr)
    : descriptor_(descriptor),
      view_ptr_(view_ptr),
      entity_id_ptr_(entity_id_ptr) {}

ArchetypeDataBuffer::ConstView::ConstView(const View& view)
    : descriptor_(view.descriptor_),
      view_ptr_(view.view_ptr_),
      entity_id_ptr_(view.entity_id_ptr_) {}

const void* ArchetypeDataBuffer::ConstView::TryGet(ComponentId id) const {
  auto it = descriptor_->offset_map().TryFind(id);
  if (!it) {
    return nullptr;
  }
  const auto& offset = it->val();
  return view_ptr_ + offset;
}

const EntityId& ArchetypeDataBuffer::ConstView::entity_id() const {
  return *entity_id_ptr_;
}

ArchetypeDataBuffer::View::View(const ArchetypeDescriptor* descriptor,
                                std::byte* view_ptr, EntityId* entity_id_ptr)
    : descriptor_(descriptor),
      view_ptr_(view_ptr),
      entity_id_ptr_(entity_id_ptr) {}

const void* ArchetypeDataBuffer::View::TryGet(ComponentId id) const {
  auto it = descriptor_->offset_map().TryFind(id);
  if (!it) {
    return nullptr;
  }
  const auto& offset = it->val();
  return view_ptr_ + offset;
}

void* ArchetypeDataBuffer::View::TryGet(ComponentId id) {
  auto it = descriptor_->offset_map().TryFind(id);
  if (!it) {
    return nullptr;
  }
  const auto& offset = it->val();
  return view_ptr_ + offset;
}

const EntityId& ArchetypeDataBuffer::View::entity_id() const {
  return *entity_id_ptr_;
}

EntityId& ArchetypeDataBuffer::View::entity_id() { return *entity_id_ptr_; }
