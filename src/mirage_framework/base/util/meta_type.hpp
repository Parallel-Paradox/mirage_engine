#ifndef MIRAGE_FRAMEWORK_BASE_UTIL_META_TYPE
#define MIRAGE_FRAMEWORK_BASE_UTIL_META_TYPE

#include <typeindex>

#include "mirage_framework/define.hpp"

namespace mirage {

class MIRAGE_API MetaType {
 public:
  MetaType() = delete;
  MetaType(const MetaType&) = delete;
  ~MetaType() = default;

  template <typename T>
  static const MetaType* Of() {
    static MetaType meta_type(typeid(T).name(), sizeof(T));
    return &meta_type;
  }

  const char* GetName() const;
  size_t GetSize() const;

 private:
  MetaType(const char* name, size_t size);

  const char* name_{nullptr};
  size_t size_{0};
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_UTIL_META_TYPE
