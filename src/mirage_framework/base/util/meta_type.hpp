#ifndef MIRAGE_FRAMEWORK_BASE_UTIL_META_TYPE
#define MIRAGE_FRAMEWORK_BASE_UTIL_META_TYPE

#include <typeindex>

#include "mirage_framework/define.hpp"

namespace mirage {

class MIRAGE_API MetaType {
 public:
  using TypeId = void*;

  MetaType() = default;
  MetaType(const MetaType&) = default;
  ~MetaType() = default;

  template <typename T>
  static const MetaType& Of() {
    static MetaType meta_type(typeid(T).name(),
                              static_cast<TypeId>(&meta_type));
    return meta_type;
  }

  bool operator==(const MetaType& rhs) const;
  bool operator!=(const MetaType& rhs) const;

  const char* GetName() const;
  TypeId GetTypeId() const;

 private:
  MetaType(const char* name, TypeId type_id);

  const char* name_{nullptr};
  TypeId type_id_{nullptr};
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_UTIL_META_TYPE
