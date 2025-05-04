#include <gtest/gtest.h>

#include "mirage_ecs/system/resource.hpp"
#include "mirage_ecs/system/system.hpp"

using namespace mirage;
using namespace mirage::ecs;

void EmptySystem() {}

TEST(SystemTests, EmptyConstruct) {
  System empty_system = System::From(EmptySystem);
  EXPECT_TRUE(std::is_const_v<std::remove_reference_t<const int32_t&>>);
}

struct GlobalNum final : Resource {
  int32_t num{0};
};

void EditNum(const Res<GlobalNum> global_num) { global_num->num = 1; }

TEST(SystemTests, EditResource) {
  World world;
  world.InitializeResource<GlobalNum>();
  EXPECT_EQ(world.GetResource<GlobalNum>().num, 0);

  auto edit_num = System::From(EditNum);
  edit_num.Run(world);
  EXPECT_EQ(world.GetResource<GlobalNum>().num, 1);
}
