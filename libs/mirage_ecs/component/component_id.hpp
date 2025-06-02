#ifndef MIRAGE_ECS_COMPONENT_COMPONENT_ID
#define MIRAGE_ECS_COMPONENT_COMPONENT_ID

#include "mirage_base/util/hash.hpp"
#include "mirage_base/util/type_id.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/util/marker.hpp"

namespace mirage {
namespace ecs {

class ComponentMeta {
  using TypeId = base::TypeId;

 public:
  using DestructFunc = void (*)(void *ptr);
  using MoveFunc = void (*)(void *target, void *destination);

  ComponentMeta() = delete;
  MIRAGE_ECS ~ComponentMeta() = default;

  ComponentMeta(const ComponentMeta &) = delete;
  ComponentMeta(ComponentMeta &&) = delete;

  template <IsComponent T>
  static const ComponentMeta &Of() {
    static ComponentMeta meta_type(TypeId::Of<T>(), Destruct<T>, Move<T>);
    return meta_type;
  }

  MIRAGE_ECS bool operator==(const ComponentMeta &other) const;
  MIRAGE_ECS bool operator!=(const ComponentMeta &other) const;
  MIRAGE_ECS std::strong_ordering operator<=>(const ComponentMeta &other) const;

  [[nodiscard]] MIRAGE_ECS TypeId type_id() const;
  [[nodiscard]] MIRAGE_ECS DestructFunc destruct_func() const;
  [[nodiscard]] MIRAGE_ECS MoveFunc move_func() const;

 private:
  MIRAGE_ECS ComponentMeta(TypeId type_id, DestructFunc destruct_func,
                           MoveFunc move_func);

  template <typename T>
  static void Destruct(void *ptr) {
    static_cast<T *>(ptr)->~T();
  }

  template <std::move_constructible T>
  static void Move(void *target, void *destination) {
    T *typed_target = static_cast<T *>(target);
    T *typed_destination = static_cast<T *>(destination);
    new (typed_destination) T(std::move(*typed_target));
  }

  TypeId type_id_;
  DestructFunc destruct_func_{nullptr};
  MoveFunc move_func_{nullptr};
};

class MIRAGE_ECS ComponentId {
  using TypeId = base::TypeId;

 public:
  ComponentId() = delete;
  ~ComponentId() = default;

  ComponentId(const ComponentId &) = default;
  ComponentId &operator=(const ComponentId &) = default;

  explicit ComponentId(const ComponentMeta &component_meta);

  template <IsComponent T>
  static ComponentId Of() {
    return ComponentId(ComponentMeta::Of<T>());
  }

  bool operator==(const ComponentId &other) const;
  bool operator!=(const ComponentId &other) const;
  std::strong_ordering operator<=>(const ComponentId &other) const;

  bool operator==(const TypeId &other) const;

  [[nodiscard]] TypeId type_id() const;
  [[nodiscard]] ComponentMeta::DestructFunc destruct_func() const;
  [[nodiscard]] ComponentMeta::MoveFunc move_func() const;

 private:
  const ComponentMeta *component_meta_{nullptr};
};

}  // namespace ecs

template <>
struct base::Hash<ecs::ComponentId> {
  size_t operator()(const ecs::ComponentId &component_id) const {
    return component_id.type_id().hash_code();
  }

  size_t operator()(const base::TypeId &type_id) const {
    return type_id.hash_code();
  }
};

}  // namespace mirage

#endif  // MIRAGE_ECS_COMPONENT_COMPONENT_ID
