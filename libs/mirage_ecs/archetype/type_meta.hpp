#ifndef MIRAGE_ECS_META_TYPE
#define MIRAGE_ECS_META_TYPE

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
    static TypeMeta meta_type(reinterpret_cast<size_t>(&meta_type), sizeof(T));
    return &meta_type;
  }

  [[nodiscard]] size_t GetTypeId() const;
  [[nodiscard]] size_t GetTypeSize() const;

 private:
  explicit TypeMeta(size_t type_id, size_t type_size);

  size_t type_id_{0};
  size_t type_size_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_META_TYPE
