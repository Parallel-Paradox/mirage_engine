#include <gtest/gtest.h>

#include "mirage_ecs/entity/archetype.hpp"

using namespace mirage::ecs;
using namespace mirage::base;

namespace {

using SharedDescriptor = SharedLocal<ArchetypeDescriptor>;

class ArchetypeTests : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}

  SharedDescriptor desc_;
  AlignedBufferPool pool_;
  Archetype archetype_;
};

}  // namespace
