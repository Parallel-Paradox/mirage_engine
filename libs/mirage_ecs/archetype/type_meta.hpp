#ifndef MIRAGE_ECS_META_TYPE
#define MIRAGE_ECS_META_TYPE

#include <cstddef>
#include <typeinfo>

#include "mirage_ecs/define.hpp"

namespace mirage::ecs {

class MIRAGE_ECS TypeMeta {
 public:
  TypeMeta() = delete;
  ~TypeMeta() = default;

  TypeMeta(const TypeMeta &) = delete;
  TypeMeta(TypeMeta &&) = delete;

  template <typename T>
  static const TypeMeta *Of() {
    static TypeMeta meta_type(typeid(T).name(),
                              reinterpret_cast<size_t>(&meta_type), sizeof(T));
    return &meta_type;
  }

  [[nodiscard]] const char *GetTypeName() const;
  [[nodiscard]] size_t GetTypeId() const;
  [[nodiscard]] size_t GetTypeSize() const;

 private:
  explicit TypeMeta(const char *type_name, size_t type_id, size_t type_size);

  const char *type_name_{nullptr};
  size_t type_id_{0};
  size_t type_size_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_META_TYPE
