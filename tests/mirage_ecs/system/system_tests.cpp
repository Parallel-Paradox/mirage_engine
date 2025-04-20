#include <gtest/gtest.h>

#include "mirage_ecs/system/query.hpp"
#include "mirage_ecs/system/system.hpp"

using namespace mirage;
using namespace mirage::ecs;

void EmptySystem() {}

TEST(SystemTests, Construct) { System system = System::From(EmptySystem); }
