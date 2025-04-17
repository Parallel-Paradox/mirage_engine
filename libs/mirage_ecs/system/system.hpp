#ifndef MIRAGE_ECS_SYSTEM_SYSTEM
#define MIRAGE_ECS_SYSTEM_SYSTEM

#include <functional>

#include "mirage_ecs/define.hpp"
#include "mirage_ecs/system/context.hpp"

namespace mirage::ecs {

class System {
 public:
 private:
  std::function<void(Context& context)> system_func_;
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SYSTEM_SYSTEM
