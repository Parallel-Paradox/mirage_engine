#ifndef MIRAGE_FRAMEWORK_BASE_CONTAINER_HASH_MAP
#define MIRAGE_FRAMEWORK_BASE_CONTAINER_HASH_MAP

#include "mirage_framework/base/container/concept.hpp"
#include "mirage_framework/base/util/hash.hpp"
#include "mirage_framework/define.hpp"

namespace mirage {

template <HashKeyType Key, BasicValueType Val>
class HashMap {
 public:
 private:
  size_t size_{0};
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_CONTAINER_HASH_MAP
