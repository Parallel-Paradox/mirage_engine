#ifndef MIRAGE_ECS_META_TYPE
#define MIRAGE_ECS_META_TYPE

#include "mirage_ecs/define.hpp"

namespace mirage::ecs {

class MIRAGE_ECS MetaType {
 public:
  MetaType() = delete;
  ~MetaType() = default;

  MetaType(const MetaType &) = delete;
  MetaType(MetaType &&) = delete;

  template <typename T>
  static const MetaType *Of() {
    static MetaType meta_type(reinterpret_cast<size_t>(&meta_type));
    return &meta_type;
  }

  [[nodiscard]] size_t GetTypeId() const;

 private:
  explicit MetaType(size_t type_id);

  size_t type_id_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_META_TYPE
