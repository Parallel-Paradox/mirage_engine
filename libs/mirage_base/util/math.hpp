#ifndef MIRAGE_BASE_UTIL_MATH
#define MIRAGE_BASE_UTIL_MATH

#include <cstddef>
#include <cstdint>

#include "mirage_base/define/export.hpp"

namespace mirage::base {

MIRAGE_BASE bool IsPowerOfTwo(size_t value);
MIRAGE_BASE uint16_t FixToPowerOfTwo(uint16_t val);

}  // namespace mirage::base

#endif  // MIRAGE_BASE_UTIL_MATH
