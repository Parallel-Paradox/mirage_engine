#include "mirage_ecs/entity/buffer/archetype_data_buffer.hpp"

#include "mirage_base/define/check.hpp"

using namespace mirage::ecs;

ArchetypeDataBuffer::ArchetypeDataBuffer(Buffer&& buffer,
                                         SharedDescriptor&& descriptor)
    : descriptor_(std::move(descriptor)), buffer_(std::move(buffer)) {
  MIRAGE_DCHECK(buffer_.align() >= alignof(EntityId));
  MIRAGE_DCHECK(buffer_.align() >= descriptor_->align());
  size_ = 0;
  const auto capacity = buffer_.size() / unit_size();
  capacity_ = static_cast<uint16_t>(capacity);
}

ArchetypeDataBuffer::~ArchetypeDataBuffer() { std::move(*this).TakeBuffer(); }

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
    const uint16_t index) const {
  MIRAGE_DCHECK(index < size_);
  const auto* view_ptr = buffer_.ptr() + index * descriptor_->size();
  const auto* entity_id_ptr =
      reinterpret_cast<const EntityId*>(buffer_.ptr() + buffer_.size()) -
      (capacity_ - index);
  return ConstView(descriptor_.raw_ptr(), view_ptr, entity_id_ptr);
}

ArchetypeDataBuffer::View ArchetypeDataBuffer::operator[](
    const uint16_t index) {
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

  ++size_;
}

void ArchetypeDataBuffer::Push(View&& view) {
  MIRAGE_DCHECK(size_ < capacity_);
  auto* view_ptr = buffer_.ptr() + size_ * descriptor_->size();
  for (const auto& entry : descriptor_->offset_map()) {
    const auto& component_id = entry.key();
    const auto& offset = entry.val();

    void* component_ptr = view.TryGet(component_id);
    if (!component_ptr) {
      continue;
    }
    component_id.move_func()(component_ptr, view_ptr + offset);
  }

  auto* entity_id_ptr =
      reinterpret_cast<EntityId*>(buffer_.ptr() + buffer_.size()) -
      (capacity_ - size_);
  *entity_id_ptr = view.entity_id();
  view.entity_id().Reset();

  ++size_;
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

void ArchetypeDataBuffer::Reserve(size_t byte_size) {
  if (byte_size <= buffer_.size()) {
    return;
  }

  ArchetypeDataBuffer old_buffer = std::move(*this);
  const auto old_buffer_size = old_buffer.size_;
  new (this) ArchetypeDataBuffer({byte_size, old_buffer.buffer_.align()},
                                 old_buffer.descriptor_.Clone());
  for (auto i = 0; i < old_buffer_size; ++i) {
    Push(old_buffer[i]);
  }
}

ArchetypeDataBuffer::Buffer ArchetypeDataBuffer::TakeBuffer() && {
  Clear();
  descriptor_ = nullptr;
  capacity_ = 0;
  return std::move(buffer_);
}

const ArchetypeDataBuffer::SharedDescriptor& ArchetypeDataBuffer::descriptor()
    const {
  return descriptor_;
}

uint16_t ArchetypeDataBuffer::size() const { return size_; }

uint16_t ArchetypeDataBuffer::capacity() const { return capacity_; }

bool ArchetypeDataBuffer::is_full() const { return size_ == capacity_; }

size_t ArchetypeDataBuffer::unit_size() const {
  return descriptor_->size() + sizeof(EntityId);
}

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

const void* ArchetypeDataBuffer::ConstView::TryGet(const ComponentId id) const {
  const auto it = descriptor_->offset_map().TryFind(id);
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

const void* ArchetypeDataBuffer::View::TryGet(const ComponentId id) const {
  const auto it = descriptor_->offset_map().TryFind(id);
  if (!it) {
    return nullptr;
  }
  const auto& offset = it->val();
  return view_ptr_ + offset;
}

void* ArchetypeDataBuffer::View::TryGet(const ComponentId id) {
  const auto it = descriptor_->offset_map().TryFind(id);
  if (!it) {
    return nullptr;
  }
  const auto& offset = it->val();
  return view_ptr_ + offset;
}

std::byte* ArchetypeDataBuffer::View::view_ptr() { return view_ptr_; }

const EntityId& ArchetypeDataBuffer::View::entity_id() const {
  return *entity_id_ptr_;
}

EntityId& ArchetypeDataBuffer::View::entity_id() { return *entity_id_ptr_; }
