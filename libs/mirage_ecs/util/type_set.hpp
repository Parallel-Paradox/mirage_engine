#ifndef MIRAGE_ECS_UTIL_TYPE_SET
#define MIRAGE_ECS_UTIL_TYPE_SET

#include "mirage_base/container/array.hpp"
#include "mirage_ecs/define.hpp"
#include "mirage_ecs/util/type_id.hpp"

namespace mirage {
namespace ecs {

class TypeSet {
 public:
  MIRAGE_ECS TypeSet() = default;
  MIRAGE_ECS ~TypeSet() = default;

  MIRAGE_ECS TypeSet(const TypeSet &) = delete;
  MIRAGE_ECS TypeSet &operator=(const TypeSet &) = delete;

  MIRAGE_ECS TypeSet(TypeSet &&other) noexcept;
  MIRAGE_ECS TypeSet &operator=(TypeSet &&other) noexcept;

  MIRAGE_ECS [[nodiscard]] TypeSet Clone() const;

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

  MIRAGE_ECS bool operator==(const TypeSet &other) const;

 private:
  base::Array<TypeId> type_array_{};
  size_t mask_{0};
};

template <typename... Ts>
TypeSet TypeSet::New() {
  TypeSet set;
  set.type_array_.Reserve(sizeof...(Ts));
  (set.AddType<Ts>(), ...);
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
