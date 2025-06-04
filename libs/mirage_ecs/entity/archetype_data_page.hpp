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

  class View;
  class Slice;

  ArchetypeDataPage() = delete;
  MIRAGE_ECS ArchetypeDataPage(size_t buffer_size, size_t align);

  MIRAGE_ECS ~ArchetypeDataPage();

  ArchetypeDataPage(const ArchetypeDataPage&) = delete;
  ArchetypeDataPage& operator=(const ArchetypeDataPage&) = delete;

  MIRAGE_ECS ArchetypeDataPage(ArchetypeDataPage&&);
  MIRAGE_ECS ArchetypeDataPage& operator=(ArchetypeDataPage&&);

  MIRAGE_ECS void Initialize(SharedDescriptor descriptor);
  MIRAGE_ECS void Reset();

  [[nodiscard]] MIRAGE_ECS bool Push(ComponentBundle& bundle);
  [[nodiscard]] MIRAGE_ECS bool Push(Slice&& slice);
  MIRAGE_ECS Slice SwapPop(size_t index);
  MIRAGE_ECS void SwapRemove(size_t index);

  MIRAGE_ECS void Clear();

  MIRAGE_ECS View operator[](size_t index);
  MIRAGE_ECS const View operator[](size_t index) const;

  [[nodiscard]] MIRAGE_ECS bool is_initialized() const;

  [[nodiscard]] MIRAGE_ECS const SharedDescriptor& descriptor() const;
  [[nodiscard]] MIRAGE_ECS size_t capacity() const;
  [[nodiscard]] MIRAGE_ECS size_t size() const;

  [[nodiscard]] MIRAGE_ECS const Buffer& buffer() const;

 private:
  SharedDescriptor descriptor_{nullptr};
  size_t capacity_{0};
  size_t size_{0};

  Buffer buffer_;
};

class MIRAGE_ECS ArchetypeDataPage::View {
 public:
  View() = default;
  ~View() = default;

  View(const View&) = default;
  View& operator=(const View&) = default;

  View(View&&) noexcept = default;
  View& operator=(View&&) noexcept = default;

  explicit operator bool() const;
  bool is_null() const;

  template <IsComponent T>
  const T* TryGet() const;
  template <IsComponent T>
  const T& Get() const;

  template <IsComponent T>
  T* TryGet();
  template <IsComponent T>
  T& Get();

 private:
  View(ArchetypeDescriptor* descriptor, std::byte* view_ptr);

  void* TryGetImpl(ComponentId id) const;

  ArchetypeDescriptor* descriptor_{nullptr};
  std::byte* view_ptr_{nullptr};
};

class ArchetypeDataPage::Slice {
 public:
  MIRAGE_ECS Slice() = default;
  MIRAGE_ECS ~Slice();

  Slice(const Slice&) = delete;
  Slice& operator=(const Slice&) = delete;

  MIRAGE_ECS Slice(Slice&& other) noexcept;
  MIRAGE_ECS Slice& operator=(Slice&& other) noexcept;

  explicit operator bool() const;
  bool is_null() const;

  MIRAGE_ECS View view();
  MIRAGE_ECS const View view() const;

 private:
  Slice(SharedDescriptor descriptor, std::byte* slice_ptr);

  SharedDescriptor descriptor_{nullptr};
  std::byte* slice_ptr_{nullptr};
};

template <IsComponent T>
const T* ArchetypeDataPage::View::TryGet() const {
  return TryGetImpl(ComponentId::Of<T>());
}

template <IsComponent T>
const T& ArchetypeDataPage::View::Get() const {
  return *TryGet<T>();
}

template <IsComponent T>
T* ArchetypeDataPage::View::TryGet() {
  return TryGetImpl(ComponentId::Of<T>());
}

template <IsComponent T>
T& ArchetypeDataPage::View::Get() {
  return *TryGet<T>();
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE
