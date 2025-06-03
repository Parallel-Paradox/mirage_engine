#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE
#define MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"

namespace mirage::ecs {

class ArchetypeDataPage {
  using SharedLocalDescriptor = base::SharedLocal<ArchetypeDescriptor>;

 public:
  class View;
  class Slice;

  ArchetypeDataPage() = delete;
  MIRAGE_ECS ~ArchetypeDataPage();

  ArchetypeDataPage(const ArchetypeDataPage&) = delete;
  ArchetypeDataPage& operator=(const ArchetypeDataPage&) = delete;

  MIRAGE_ECS ArchetypeDataPage(ArchetypeDataPage&&);
  MIRAGE_ECS ArchetypeDataPage& operator=(ArchetypeDataPage&&);

  MIRAGE_ECS ArchetypeDataPage(size_t byte_size);

  MIRAGE_ECS void Initialize(SharedLocalDescriptor descriptor);
  MIRAGE_ECS void Reset();

  [[nodiscard]] MIRAGE_ECS bool Push(ComponentBundle& bundle);
  [[nodiscard]] MIRAGE_ECS bool Push(Slice&& slice);
  MIRAGE_ECS Slice SwapPop(size_t index);
  MIRAGE_ECS void SwapRemove(size_t index);

  MIRAGE_ECS void Clear();

  MIRAGE_ECS View operator[](size_t index);
  MIRAGE_ECS const View operator[](size_t index) const;

  [[nodiscard]] MIRAGE_ECS const SharedLocalDescriptor& descriptor() const;
  [[nodiscard]] MIRAGE_ECS size_t capacity() const;
  [[nodiscard]] MIRAGE_ECS size_t size() const;

  [[nodiscard]] MIRAGE_ECS std::byte* buffer_ptr() const;
  [[nodiscard]] MIRAGE_ECS size_t align_padding() const;
  [[nodiscard]] MIRAGE_ECS size_t byte_size() const;

 private:
  SharedLocalDescriptor descriptor_{nullptr};
  size_t capacity_{0};
  size_t size_{0};

  std::byte* buffer_ptr_{nullptr};
  size_t align_padding_{0};
  size_t byte_size_{0};
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

  template <IsComponent T>
  T* TryGetImpl() const;

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
  Slice(SharedLocalDescriptor descriptor, std::byte* slice_ptr);

  SharedLocalDescriptor descriptor_{nullptr};
  std::byte* slice_ptr_{nullptr};
};

template <IsComponent T>
const T* ArchetypeDataPage::View::TryGet() const {
  return TryGetImpl<T>();
}

template <IsComponent T>
const T& ArchetypeDataPage::View::Get() const {
  return *TryGet<T>();
}

template <IsComponent T>
T* ArchetypeDataPage::View::TryGet() {
  return TryGetImpl<T>();
}

template <IsComponent T>
T& ArchetypeDataPage::View::Get() {
  return *TryGet<T>();
}

template <IsComponent T>
T* ArchetypeDataPage::View::TryGetImpl() const {
  const auto& offset_map = descriptor_->offset_map();
  ArchetypeDescriptor::OffsetMap::ConstIterator iter =
      offset_map.TryFind(base::TypeId::Of<T>());
  if (iter == offset_map.end()) {
    return nullptr;
  }
  return static_cast<T*>(view_ptr_ + iter->val());
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE
