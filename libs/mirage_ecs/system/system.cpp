#include "mirage_ecs/system/system.hpp"

using namespace mirage::ecs;

void System::Run(Context &context) { system_func_(context); }
