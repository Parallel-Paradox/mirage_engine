#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE
#define MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE

#include <compare>
#include <iterator>

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/buffer/aligned_buffer.hpp"
#include "mirage_base/container/array.hpp"
#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/component/component_id.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"
#include "mirage_ecs/entity/entity_id.hpp"

namespace mirage::ecs {

class ArchetypeDataPage {
  template <typename T>
  using Array = base::Array<T>;

 public:
  using SharedDescriptor = base::SharedLocal<ArchetypeDescriptor>;
  using Buffer = base::AlignedBuffer;

  class ConstView;
  class ConstIterator;
  class View;
  class Iterator;
  class Courier;

  ArchetypeDataPage() = default;
  MIRAGE_ECS ArchetypeDataPage(size_t buffer_size, size_t align);

  MIRAGE_ECS ~ArchetypeDataPage();

  ArchetypeDataPage(const ArchetypeDataPage&) = delete;
  ArchetypeDataPage& operator=(const ArchetypeDataPage&) = delete;

  MIRAGE_ECS ArchetypeDataPage(ArchetypeDataPage&&) noexcept = default;
  MIRAGE_ECS ArchetypeDataPage& operator=(ArchetypeDataPage&&) noexcept =
      default;

  MIRAGE_ECS void Initialize(SharedDescriptor&& descriptor);
  MIRAGE_ECS void Reset();

  MIRAGE_ECS void Reserve(size_t new_buffer_size);

  // Return sparse index, -1 if failed
  MIRAGE_ECS int32_t Push(const EntityId& id, ComponentBundle& bundle);
  MIRAGE_ECS int32_t Push(View& view);

  [[nodiscard]] MIRAGE_ECS Courier Take(int32_t sparse_id);
  [[nodiscard]] MIRAGE_ECS Courier
  TakeMany(const Array<int32_t>& sparse_id_array);

  [[nodiscard]] MIRAGE_ECS Courier Take(int32_t sparse_id,
                                        const SharedDescriptor& descriptor);
  [[nodiscard]] MIRAGE_ECS Courier
  TakeMany(const Array<int32_t>& sparse_id_array,
           const SharedDescriptor& descriptor);

  MIRAGE_ECS void Remove(int32_t sparse_id);
  MIRAGE_ECS void RemoveMany(const Array<int32_t>& sparse_id_array);

  MIRAGE_ECS void Clear();

  MIRAGE_ECS ConstView operator[](int32_t sparse_id) const;
  MIRAGE_ECS View operator[](int32_t sparse_id);

  // Iterate with dense index
  [[nodiscard]] MIRAGE_ECS ConstIterator begin() const;
  [[nodiscard]] MIRAGE_ECS ConstIterator end() const;
  MIRAGE_ECS Iterator begin();
  MIRAGE_ECS Iterator end();

  [[nodiscard]] MIRAGE_ECS bool is_initialized() const;

  [[nodiscard]] MIRAGE_ECS const SharedDescriptor& descriptor() const;

  [[nodiscard]] MIRAGE_ECS const Array<EntityId>& entity_id_array() const;
  [[nodiscard]] MIRAGE_ECS size_t capacity() const;
  [[nodiscard]] MIRAGE_ECS size_t size() const;

  [[nodiscard]] MIRAGE_ECS const Buffer& buffer() const;
  MIRAGE_ECS Buffer& buffer();

  [[nodiscard]] MIRAGE_ECS const Array<int32_t>& dense() const;
  [[nodiscard]] MIRAGE_ECS const Array<int32_t>& sparse() const;
  [[nodiscard]] MIRAGE_ECS const Array<int32_t>& hole() const;

 private:
  [[nodiscard]] MIRAGE_ECS Array<int32_t> MapSparseToDense(
      const Array<int32_t>& sparse_id_array) const;
  MIRAGE_ECS void SwapRemoveDense(int32_t dense_id);
  MIRAGE_ECS void SwapRemoveManyDense(Array<int32_t>&& dense_id_array);

  SharedDescriptor descriptor_{nullptr};
  Array<EntityId> entity_id_array_;
  Buffer buffer_;

  Array<int32_t> dense_;
  Array<int32_t> sparse_;
  Array<int32_t> hole_;
};

class MIRAGE_ECS ArchetypeDataPage::ConstView {
 public:
  ConstView() = default;
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
  [[nodiscard]] bool is_null() const;

 private:
  friend class ArchetypeDataPage;
  friend class ConstIterator;

  ConstView(const ArchetypeDescriptor* descriptor, const std::byte* view_ptr,
            Array<EntityId>::ConstIterator entity_id_iter);

  const ArchetypeDescriptor* descriptor_{nullptr};
  const std::byte* view_ptr_{nullptr};
  Array<EntityId>::ConstIterator entity_id_iter_{nullptr};
};

class MIRAGE_ECS ArchetypeDataPage::ConstIterator {
 public:
  using iterator_concept = std::random_access_iterator_tag;
  using iterator_type = ConstIterator;
  using difference_type = ptrdiff_t;
  using value_type = ConstView;
  using pointer = ConstView;
  using reference = ConstView;

  ConstIterator() = default;
  explicit ConstIterator(const ArchetypeDataPage& page);
  explicit ConstIterator(const Courier& courier);
  ~ConstIterator() = default;

  ConstIterator(const ConstIterator&) = default;
  ConstIterator& operator=(const ConstIterator&) = default;

  ConstIterator(ConstIterator&&) noexcept = default;
  ConstIterator& operator=(ConstIterator&&) noexcept = default;

  ConstIterator(std::nullptr_t);  // NOLINT: Convert from nullptr
  ConstIterator& operator=(std::nullptr_t);

  reference operator*() const;
  pointer operator->() const;
  reference operator[](difference_type diff) const;
  iterator_type& operator++();
  iterator_type operator++(int);
  iterator_type& operator--();
  iterator_type operator--(int);
  iterator_type& operator+=(difference_type diff);
  iterator_type operator+(difference_type diff) const;
  friend iterator_type operator+(ptrdiff_t diff, const iterator_type& iter);
  iterator_type& operator-=(difference_type diff);
  iterator_type operator-(difference_type diff) const;
  difference_type operator-(const iterator_type& other) const;
  std::strong_ordering operator<=>(const iterator_type& other) const;
  bool operator==(const iterator_type& other) const;

  explicit operator bool() const;
  [[nodiscard]] bool is_null() const;

 private:
  ConstView view_;
};

class MIRAGE_ECS ArchetypeDataPage::View {
 public:
  View() = default;
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

  [[nodiscard]] const EntityId& entity_id() const;
  [[nodiscard]] bool is_null() const;

 private:
  friend class ArchetypeDataPage;
  friend class Iterator;

  View(const ArchetypeDescriptor* descriptor, std::byte* view_ptr,
       Array<EntityId>::ConstIterator entity_id_iter);

  const ArchetypeDescriptor* descriptor_{nullptr};
  std::byte* view_ptr_{nullptr};
  Array<EntityId>::ConstIterator entity_id_iter_{nullptr};
};

class MIRAGE_ECS ArchetypeDataPage::Iterator {
 public:
  using iterator_concept = std::random_access_iterator_tag;
  using iterator_type = Iterator;
  using difference_type = ptrdiff_t;
  using value_type = View;
  using pointer = View;
  using reference = View;

  Iterator() = default;
  explicit Iterator(ArchetypeDataPage& page);
  explicit Iterator(Courier& courier);
  ~Iterator() = default;

  Iterator(const Iterator&) = default;
  Iterator& operator=(const Iterator&) = default;

  Iterator(Iterator&&) noexcept = default;
  Iterator& operator=(Iterator&&) noexcept = default;

  Iterator(std::nullptr_t);  // NOLINT: Convert from nullptr
  Iterator& operator=(std::nullptr_t);

  reference operator*() const;
  pointer operator->() const;
  reference operator[](difference_type diff) const;
  iterator_type& operator++();
  iterator_type operator++(int);
  iterator_type& operator--();
  iterator_type operator--(int);
  iterator_type& operator+=(difference_type diff);
  iterator_type operator+(difference_type diff) const;
  friend iterator_type operator+(ptrdiff_t diff, const iterator_type& iter);
  iterator_type& operator-=(difference_type diff);
  iterator_type operator-(difference_type diff) const;
  difference_type operator-(const iterator_type& other) const;
  std::strong_ordering operator<=>(const iterator_type& other) const;
  bool operator==(const iterator_type& other) const;

  explicit operator bool() const;
  [[nodiscard]] bool is_null() const;

 private:
  View view_;
};

class ArchetypeDataPage::Courier {
 public:
  Courier() = delete;
  MIRAGE_ECS ~Courier();

  Courier(const Courier&) = delete;
  Courier& operator=(const Courier&) = delete;

  MIRAGE_ECS Courier(Courier&& other) noexcept = default;
  MIRAGE_ECS Courier& operator=(Courier&& other) noexcept = default;

  explicit operator bool() const;
  [[nodiscard]] bool is_null() const;

  [[nodiscard]] MIRAGE_ECS ConstIterator begin() const;
  [[nodiscard]] MIRAGE_ECS ConstIterator end() const;
  MIRAGE_ECS Iterator begin();
  MIRAGE_ECS Iterator end();

  [[nodiscard]] MIRAGE_ECS const SharedDescriptor& descriptor() const;

  [[nodiscard]] MIRAGE_ECS const Array<EntityId>& entity_id_array() const;
  [[nodiscard]] MIRAGE_ECS size_t size() const;
  [[nodiscard]] MIRAGE_ECS ptrdiff_t ssize() const;

  [[nodiscard]] MIRAGE_ECS const Buffer& buffer() const;
  MIRAGE_ECS Buffer& buffer();

 private:
  friend class ArchetypeDataPage;
  MIRAGE_ECS Courier(const SharedDescriptor& descriptor,
                     ArchetypeDataPage& page,
                     const Array<int32_t>& dense_id_array);

  SharedDescriptor descriptor_{nullptr};
  Array<EntityId> entity_id_array_;
  Buffer buffer_;
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE
