#ifndef MIRAGE_ECS_ARCHETYPE_DATA_BUFFER
#define MIRAGE_ECS_ARCHETYPE_DATA_BUFFER

#include <cstdint>

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/buffer/aligned_buffer.hpp"
#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"
#include "mirage_ecs/entity/entity_id.hpp"

namespace mirage::ecs {

class ArchetypeDataBuffer {
  using Buffer = base::AlignedBuffer;
  using SharedDescriptor = base::SharedLocal<ArchetypeDescriptor>;

 public:
  class ConstView;
  class View;

  MIRAGE_ECS ArchetypeDataBuffer() = default;
  MIRAGE_ECS ArchetypeDataBuffer(Buffer&& buffer,
                                 SharedDescriptor&& descriptor);
  MIRAGE_ECS ~ArchetypeDataBuffer();

  ArchetypeDataBuffer(const ArchetypeDataBuffer&) = delete;
  ArchetypeDataBuffer& operator=(const ArchetypeDataBuffer&) = delete;

  MIRAGE_ECS ArchetypeDataBuffer(ArchetypeDataBuffer&&) noexcept;
  MIRAGE_ECS ArchetypeDataBuffer& operator=(ArchetypeDataBuffer&&) noexcept;

  MIRAGE_ECS ConstView operator[](uint16_t index) const;
  MIRAGE_ECS View operator[](uint16_t index);

  MIRAGE_ECS void Push(const EntityId& id, ComponentBundle& bundle);
  MIRAGE_ECS void Push(View&& view);

  MIRAGE_ECS void RemoveTail();
  MIRAGE_ECS void Clear();
  MIRAGE_ECS void Reserve(size_t byte_size);
  MIRAGE_ECS Buffer TakeBuffer() &&;

  [[nodiscard]] MIRAGE_ECS const SharedDescriptor& descriptor() const;

  [[nodiscard]] MIRAGE_ECS const Buffer& buffer() const;
  [[nodiscard]] MIRAGE_ECS uint16_t size() const;
  [[nodiscard]] MIRAGE_ECS uint16_t capacity() const;
  [[nodiscard]] MIRAGE_ECS bool is_full() const;
  [[nodiscard]] MIRAGE_ECS static size_t unit_size(
      ArchetypeDescriptor& descriptor);

 private:
  SharedDescriptor descriptor_{nullptr};

  Buffer buffer_;
  uint16_t size_{0};
  uint16_t capacity_{0};
};

class MIRAGE_ECS ArchetypeDataBuffer::ConstView {
 public:
  ConstView() = default;
  ConstView(const ArchetypeDescriptor* descriptor, const std::byte* view_ptr,
            const EntityId* entity_id_ptr);
  ConstView(const View& view);  // NOLINT: Convert from View

  ~ConstView() = default;

  ConstView(const ConstView&) = default;
  ConstView& operator=(const ConstView&) = default;

  ConstView(ConstView&&) noexcept = default;
  ConstView& operator=(ConstView&&) noexcept = default;

  [[nodiscard]] const void* TryGet(ComponentId id) const;

  template <IsComponent T>
  const T* TryGet() const {
    return static_cast<T*>(TryGet(ComponentId::Of<T>()));
  }

  template <IsComponent T>
  const T& Get() const {
    return *TryGet<T>();
  }

  [[nodiscard]] const EntityId& entity_id() const;

 private:
  const ArchetypeDescriptor* descriptor_{nullptr};
  const std::byte* view_ptr_{nullptr};
  const EntityId* entity_id_ptr_{nullptr};
};

class MIRAGE_ECS ArchetypeDataBuffer::View {
 public:
  View() = default;
  View(const ArchetypeDescriptor* descriptor, std::byte* view_ptr,
       EntityId* entity_id_ptr);

  ~View() = default;

  View(const View&) = default;
  View& operator=(const View&) = default;

  View(View&&) noexcept = default;
  View& operator=(View&&) noexcept = default;

  [[nodiscard]] const void* TryGet(ComponentId id) const;
  [[nodiscard]] void* TryGet(ComponentId id);

  template <IsComponent T>
  const T* TryGet() const {
    return static_cast<T*>(TryGet(ComponentId::Of<T>()));
  }

  template <IsComponent T>
  const T& Get() const {
    return *TryGet<T>();
  }

  template <IsComponent T>
  T* TryGet() {
    return static_cast<T*>(TryGet(ComponentId::Of<T>()));
  }

  template <IsComponent T>
  T& Get() {
    return *TryGet<T>();
  }

  std::byte* view_ptr();

  EntityId& entity_id();
  [[nodiscard]] const EntityId& entity_id() const;

 private:
  friend class ConstView;

  const ArchetypeDescriptor* descriptor_{nullptr};
  std::byte* view_ptr_{nullptr};
  EntityId* entity_id_ptr_{nullptr};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ARCHETYPE_DATA_BUFFER
