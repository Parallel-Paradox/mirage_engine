#ifndef MIRAGE_BASE_UTIL_TYPE_ID
#define MIRAGE_BASE_UTIL_TYPE_ID

#include <cstddef>
#include <typeindex>

#include "mirage_base/define/export.hpp"
#include "mirage_base/util/hash.hpp"

namespace mirage::base {

class TypeMeta {
 public:
  MIRAGE_BASE TypeMeta() = delete;
  MIRAGE_BASE ~TypeMeta() = default;

  MIRAGE_BASE TypeMeta(const TypeMeta &) = delete;
  MIRAGE_BASE TypeMeta(TypeMeta &&) = delete;

  template <typename T>
  static const TypeMeta &Of() {
    static TypeMeta meta_type(typeid(T), sizeof(T), alignof(T));
    return meta_type;
  }

  MIRAGE_BASE bool operator==(const TypeMeta &other) const;
  MIRAGE_BASE bool operator!=(const TypeMeta &other) const;
  MIRAGE_BASE std::strong_ordering operator<=>(const TypeMeta &other) const;

  [[nodiscard]] MIRAGE_BASE std::type_index type_index() const;
  [[nodiscard]] MIRAGE_BASE const char *type_name() const;
  [[nodiscard]] MIRAGE_BASE size_t type_size() const;
  [[nodiscard]] MIRAGE_BASE size_t type_align() const;
  [[nodiscard]] MIRAGE_BASE size_t hash_code() const;
  [[nodiscard]] MIRAGE_BASE size_t bit_flag() const;

 private:
  MIRAGE_BASE TypeMeta(std::type_index type_index, size_t type_size,
                       size_t type_align);

  std::type_index type_index_;
  size_t type_size_{0};
  size_t type_align_{0};
  size_t hash_code_{0};
};

class MIRAGE_BASE TypeId {
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

  [[nodiscard]] std::type_index type_index() const;
  [[nodiscard]] const char *type_name() const;
  [[nodiscard]] size_t type_size() const;
  [[nodiscard]] size_t type_align() const;
  [[nodiscard]] size_t hash_code() const;
  [[nodiscard]] size_t bit_flag() const;

 private:
  const TypeMeta *type_meta_{nullptr};
};

template <>
struct base::Hash<TypeId> {
  size_t operator()(const TypeId &type_id) const { return type_id.hash_code(); }
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_UTIL_TYPE_ID
