#include "mirage_ecs/system/system.hpp"

#include "mirage_base/define/check.hpp"
#include "mirage_ecs/system/system_context.hpp"

using namespace mirage::ecs;

void System::Run(World& world) { system_func_(world, context_); }

System::System(SystemFunc&& system_func, base::Owned<SystemContext>&& context)
    : system_func_(std::move(system_func)), context_(std::move(context)) {
  MIRAGE_DCHECK(system_func_);
  MIRAGE_DCHECK(context_ != nullptr);
}
