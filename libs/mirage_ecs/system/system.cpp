#include "mirage_ecs/system/system.hpp"

#include "mirage_ecs/system/system_context.hpp"

using namespace mirage::ecs;

void System::Run(World& world) { system_func_(world, context_); }

SystemContext& System::context() { return context_; }

const SystemContext& System::context() const { return context_; }
