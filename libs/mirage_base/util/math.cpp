#include "mirage_base/util/math.hpp"

namespace mirage::base {

bool IsPowerOfTwo(size_t value) {
  return (value != 0) && ((value & (value - 1)) == 0);
}

uint16_t FixToPowerOfTwo(uint16_t val) {
  val |= val >> 1;
  val |= val >> 2;
  val |= val >> 4;
  val |= val >> 8;
  return val + 1;
}

}  // namespace mirage::base
