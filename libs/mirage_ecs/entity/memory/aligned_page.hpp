#ifndef MIRAGE_ECS_ALIGNED_PAGE
#define MIRAGE_ECS_ALIGNED_PAGE

#include <cstddef>

#include "mirage_ecs/define/export.hpp"

namespace mirage::ecs {

class MIRAGE_ECS AlignedPage {
 public:
 private:
  std::byte* page_ptr_{nullptr};
  size_t size_{0};
  size_t align_{0};
};

// |left or right|is head|is tail|size|

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ALIGNED_PAGE
