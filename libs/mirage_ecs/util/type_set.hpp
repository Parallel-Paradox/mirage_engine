#ifndef MIRAGE_ECS_UTIL_TYPE_SET
#define MIRAGE_ECS_UTIL_TYPE_SET

#include "mirage_base/container/array.hpp"
#include "mirage_base/util/type_id.hpp"
#include "mirage_ecs/define/export.hpp"

namespace mirage {
namespace ecs {

class TypeSet {
 public:
  using TypeId = base::TypeId;

  MIRAGE_ECS TypeSet() = default;
  MIRAGE_ECS ~TypeSet() = default;

  TypeSet(const TypeSet &) = delete;
  TypeSet &operator=(const TypeSet &) = delete;

  MIRAGE_ECS TypeSet(TypeSet &&other) noexcept;
  MIRAGE_ECS TypeSet &operator=(TypeSet &&other) noexcept;

  [[nodiscard]] MIRAGE_ECS TypeSet Clone() const;

  MIRAGE_ECS void Reserve(size_t capacity);
  MIRAGE_ECS void ShrinkToFit();

  template <typename... Ts>
  static TypeSet New();

  template <typename T>
  void AddType();
  MIRAGE_ECS void AddTypeId(TypeId type_id);

  template <typename T>
  void RemoveType();
  MIRAGE_ECS void RemoveTypeId(const TypeId &type_id);

  [[nodiscard]] MIRAGE_ECS bool With(const TypeSet &set) const;
  [[nodiscard]] MIRAGE_ECS bool With(const TypeId &type_id) const;
  [[nodiscard]] MIRAGE_ECS bool Without(const TypeSet &set) const;
  [[nodiscard]] MIRAGE_ECS bool Without(const TypeId &type_id) const;

  [[nodiscard]] MIRAGE_ECS const base::Array<TypeId> &type_array() const;
  [[nodiscard]] MIRAGE_ECS size_t mask() const;

  [[nodiscard]] MIRAGE_ECS size_t size() const;

  MIRAGE_ECS bool operator==(const TypeSet &other) const;

 private:
  base::Array<TypeId> type_array_{};
  size_t mask_{0};
};

template <typename... Ts>
TypeSet TypeSet::New() {
  TypeSet set;
  set.Reserve(sizeof...(Ts));
  (set.AddType<Ts>(), ...);
  set.ShrinkToFit();
  return set;
}

template <typename T>
void TypeSet::AddType() {
  AddTypeId(TypeId::Of<T>());
}

template <typename T>
void TypeSet::RemoveType() {
  RemoveTypeId(TypeId::Of<T>());
}

}  // namespace ecs

template <>
struct base::Hash<ecs::TypeSet> {
  size_t operator()(const ecs::TypeSet &set) const { return set.mask(); }
};

}  // namespace mirage

#endif  // MIRAGE_ECS_UTIL_TYPE_SET
