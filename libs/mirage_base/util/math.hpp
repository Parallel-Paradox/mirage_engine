#ifndef MIRAGE_BASE_UTIL_MATH
#define MIRAGE_BASE_UTIL_MATH

#include <cstddef>

namespace mirage::base {

constexpr bool IsPowerOfTwo(size_t value) {
  return (value != 0) && ((value & (value - 1)) == 0);
}

}  // namespace mirage::base

#endif  // MIRAGE_BASE_UTIL_MATH
