#ifndef MIRAGE_ECS_ENTITY_DATA_CHUNK
#define MIRAGE_ECS_ENTITY_DATA_CHUNK

#include <cstddef>

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/util/type_id.hpp"
#include "mirage_ecs/entity/component_package.hpp"
#include "mirage_ecs/entity/entity_layout.hpp"
#include "mirage_ecs/util/marker.hpp"

namespace mirage::ecs {

class EntityView;

class EntityChunk {
 public:
  // class Iterator;
  // class ConstIterator;

  EntityChunk() = delete;
  MIRAGE_ECS ~EntityChunk();

  EntityChunk(const EntityChunk &) = delete;
  EntityChunk &operator=(const EntityChunk &) = delete;

  MIRAGE_ECS EntityChunk(EntityChunk &&other) noexcept;
  MIRAGE_ECS EntityChunk &operator=(EntityChunk &&other) noexcept;

  MIRAGE_ECS EntityChunk(base::SharedLocal<EntityLayout> &&entity_layout,
                         size_t capacity);

  bool Push(ComponentPackage &component_package);
  bool SwapRemove(size_t index);
  void Clear();

  EntityView operator[](size_t index);
  const EntityView operator[](size_t index) const;

  // Iterator begin();
  // Iterator end();

  // ConstIterator begin() const;
  // ConstIterator end() const;

  [[nodiscard]] MIRAGE_ECS const EntityLayout &entity_layout() const;

  [[nodiscard]] MIRAGE_ECS size_t byte_size() const;
  [[nodiscard]] MIRAGE_ECS std::byte *raw_ptr() const;

  [[nodiscard]] MIRAGE_ECS size_t capacity() const;
  [[nodiscard]] MIRAGE_ECS size_t size() const;

 private:
  base::SharedLocal<EntityLayout> entity_layout_{nullptr};

  size_t byte_size_{0};
  std::byte *raw_ptr_{nullptr};

  size_t capacity_{0};
  size_t size_{0};
};

class EntityView {
 public:
  EntityView() = delete;
  MIRAGE_ECS ~EntityView() = default;

  MIRAGE_ECS EntityView(const EntityView &) = default;
  MIRAGE_ECS EntityView &operator=(const EntityView &) = default;

  MIRAGE_ECS EntityView(EntityView &&other) noexcept = default;
  MIRAGE_ECS EntityView &operator=(EntityView &&other) noexcept = default;

  template <IsComponent T>
  const T *TryGet() const;

  template <IsComponent T>
  const T &Get() const;

  template <IsComponent T>
  T *TryGet();

  template <IsComponent T>
  T &Get();

 private:
  friend class EntityChunk;
  MIRAGE_ECS EntityView(EntityLayout *entity_layout, std::byte *raw_ptr);

  template <IsComponent T>
  T *TryGetImpl() const;

  EntityLayout *entity_layout_{nullptr};
  std::byte *raw_ptr_{nullptr};
};

template <IsComponent T>
const T *EntityView::TryGet() const {
  return TryGetImpl<T>();
}

template <IsComponent T>
const T &EntityView::Get() const {
  return *TryGet<T>();
}

template <IsComponent T>
T *EntityView::TryGet() {
  return TryGetImpl<T>();
}

template <IsComponent T>
T &EntityView::Get() {
  return *TryGet<T>();
}

template <IsComponent T>
T *EntityView::TryGetImpl() const {
  const auto &meta_map = entity_layout_->component_meta_map();
  EntityLayout::ComponentMetaMap::ConstIterator iter =
      meta_map.TryFind(base::TypeId::Of<T>());
  if (iter == meta_map.end()) {
    return nullptr;
  }
  const auto &meta = iter->val();
  return static_cast<T *>(raw_ptr_ + meta.offset);
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_DATA_CHUNK
