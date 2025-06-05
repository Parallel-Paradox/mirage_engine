#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE
#define MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/buffer/aligned_buffer.hpp"
#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/component/component_id.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"

namespace mirage::ecs {

class ArchetypeDataPage {
 public:
  using SharedDescriptor = base::SharedLocal<ArchetypeDescriptor>;
  using Buffer = base::AlignedBuffer;

  class ConstView;
  class View;
  class Slice;

  ArchetypeDataPage() = delete;
  MIRAGE_ECS ArchetypeDataPage(size_t buffer_size, size_t align);

  MIRAGE_ECS ~ArchetypeDataPage();

  ArchetypeDataPage(const ArchetypeDataPage&) = delete;
  ArchetypeDataPage& operator=(const ArchetypeDataPage&) = delete;

  MIRAGE_ECS ArchetypeDataPage(ArchetypeDataPage&&) noexcept;
  MIRAGE_ECS ArchetypeDataPage& operator=(ArchetypeDataPage&&) noexcept;

  MIRAGE_ECS void Initialize(SharedDescriptor descriptor);
  MIRAGE_ECS void Reset();

  [[nodiscard]] MIRAGE_ECS bool Push(ComponentBundle& bundle);
  [[nodiscard]] MIRAGE_ECS bool Push(Slice&& slice);
  MIRAGE_ECS Slice SwapPop(size_t index);
  MIRAGE_ECS void SwapRemove(size_t index);

  MIRAGE_ECS void Clear();

  MIRAGE_ECS ConstView operator[](size_t index) const;
  MIRAGE_ECS View operator[](size_t index);

  [[nodiscard]] MIRAGE_ECS bool is_initialized() const;

  [[nodiscard]] MIRAGE_ECS const SharedDescriptor& descriptor() const;
  [[nodiscard]] MIRAGE_ECS size_t capacity() const;
  [[nodiscard]] MIRAGE_ECS size_t size() const;

  [[nodiscard]] MIRAGE_ECS const Buffer& buffer() const;
  [[nodiscard]] MIRAGE_ECS Buffer& buffer();

 private:
  SharedDescriptor descriptor_{nullptr};
  size_t capacity_{0};
  size_t size_{0};

  Buffer buffer_;
};

class MIRAGE_ECS ArchetypeDataPage::ConstView {
 public:
  ConstView() = default;
  ConstView(const ArchetypeDataPage& page, size_t index);
  explicit ConstView(const Slice& slice);
  ~ConstView() = default;

  ConstView(const ConstView&) = default;
  ConstView& operator=(const ConstView&) = default;

  ConstView(ConstView&&) noexcept = default;
  ConstView& operator=(ConstView&&) noexcept = default;

  explicit operator bool() const;
  [[nodiscard]] bool is_null() const;

  [[nodiscard]] const void* TryGet(ComponentId id) const;

  template <IsComponent T>
  const T* TryGet() const {
    return TryGet(ComponentId::Of<T>());
  }

  template <IsComponent T>
  const T& Get() const {
    return *TryGet<T>();
  }

 private:
  const ArchetypeDescriptor* descriptor_{nullptr};
  const std::byte* view_ptr_{nullptr};
};

class MIRAGE_ECS ArchetypeDataPage::View {
 public:
  View() = default;
  View(ArchetypeDataPage& page, size_t index);
  explicit View(Slice& slice);
  ~View() = default;

  View(const View&) = default;
  View& operator=(const View&) = default;

  View(View&&) noexcept = default;
  View& operator=(View&&) noexcept = default;

  explicit operator bool() const;
  [[nodiscard]] bool is_null() const;

  [[nodiscard]] const void* TryGet(ComponentId id) const;
  [[nodiscard]] void* TryGet(ComponentId id);

  template <IsComponent T>
  const T* TryGet() const {
    return TryGet(ComponentId::Of<T>());
  }

  template <IsComponent T>
  const T& Get() const {
    return *TryGet<T>();
  }

  template <IsComponent T>
  T* TryGet() {
    return TryGet(ComponentId::Of<T>());
  }

  template <IsComponent T>
  T& Get() {
    return *TryGet<T>();
  }

 private:
  const ArchetypeDescriptor* descriptor_{nullptr};
  std::byte* view_ptr_{nullptr};
};

class ArchetypeDataPage::Slice {
 public:
  MIRAGE_ECS Slice() = default;
  MIRAGE_ECS Slice(ArchetypeDataPage& page, size_t index);
  MIRAGE_ECS ~Slice();

  Slice(const Slice&) = delete;
  Slice& operator=(const Slice&) = delete;

  MIRAGE_ECS Slice(Slice&& other) noexcept;
  MIRAGE_ECS Slice& operator=(Slice&& other) noexcept;

  explicit operator bool() const;
  [[nodiscard]] bool is_null() const;

  [[nodiscard]] MIRAGE_ECS ConstView view() const;
  [[nodiscard]] MIRAGE_ECS View view();

  [[nodiscard]] MIRAGE_ECS const SharedDescriptor& descriptor() const;
  [[nodiscard]] MIRAGE_ECS const std::byte* slice_ptr() const;
  [[nodiscard]] MIRAGE_ECS std::byte* slice_ptr();

 private:
  SharedDescriptor descriptor_{nullptr};
  std::byte* slice_ptr_{nullptr};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE
