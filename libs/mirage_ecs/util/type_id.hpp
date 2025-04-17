#ifndef MIRAGE_ECS_ARCHETYPE_TYPE_ID
#define MIRAGE_ECS_ARCHETYPE_TYPE_ID

#include <cstddef>
#include <typeindex>

#include "mirage_base/util/hash.hpp"
#include "mirage_ecs/define.hpp"

namespace mirage {
namespace ecs {

class TypeMeta {
 public:
  MIRAGE_ECS TypeMeta() = delete;
  MIRAGE_ECS ~TypeMeta() = default;

  MIRAGE_ECS TypeMeta(const TypeMeta &) = delete;
  MIRAGE_ECS TypeMeta(TypeMeta &&) = delete;

  template <typename T>
  static const TypeMeta &Of() {
    static TypeMeta meta_type(typeid(T), sizeof(T), alignof(T));
    return meta_type;
  }

  MIRAGE_ECS bool operator==(const TypeMeta &other) const;
  MIRAGE_ECS bool operator!=(const TypeMeta &other) const;
  MIRAGE_ECS std::strong_ordering operator<=>(const TypeMeta &other) const;

  [[nodiscard]] MIRAGE_ECS const char *type_name() const;
  [[nodiscard]] MIRAGE_ECS size_t type_size() const;
  [[nodiscard]] MIRAGE_ECS size_t type_align() const;
  [[nodiscard]] MIRAGE_ECS size_t hash_code() const;
  [[nodiscard]] MIRAGE_ECS size_t bit_flag() const;

 private:
  MIRAGE_ECS TypeMeta(std::type_index type_index, size_t type_size,
                      size_t type_align);

  std::type_index type_index_;
  size_t type_size_{0};
  size_t type_align_{0};
  size_t hash_code_{0};
  size_t bit_flag_{0};
};

class MIRAGE_ECS TypeId {
 public:
  TypeId() = delete;
  ~TypeId() = default;

  TypeId(const TypeId &) = default;
  TypeId &operator=(const TypeId &) = default;

  explicit TypeId(const TypeMeta &type_meta);

  template <typename T>
  static TypeId Of() {
    return TypeId(TypeMeta::Of<T>());
  }

  bool operator==(const TypeId &other) const;
  bool operator!=(const TypeId &other) const;
  std::strong_ordering operator<=>(const TypeId &other) const;

  [[nodiscard]] const char *type_name() const;
  [[nodiscard]] size_t type_size() const;
  [[nodiscard]] size_t type_align() const;
  [[nodiscard]] size_t hash_code() const;
  [[nodiscard]] size_t bit_flag() const;

 private:
  const TypeMeta *type_meta_{nullptr};
};

}  // namespace ecs

template <>
struct base::Hash<ecs::TypeId> {
  size_t operator()(const ecs::TypeId &type_id) const {
    return type_id.hash_code();
  }
};

}  // namespace mirage

#endif  // MIRAGE_ECS_ARCHETYPE_TYPE_ID
