#include <gtest/gtest.h>

#include "mirage_base/auto_ptr/observed.hpp"
#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_ecs/entity/archetype.hpp"
#include "mirage_ecs/entity/archetype_page_pool.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

using SharedDescriptor = SharedLocal<ArchetypeDescriptor>;
using ObservedPagePool = ObservedLocal<ArchetypePagePool>;
using PagePoolObserver = LocalObserver<ArchetypePagePool>;

namespace {

struct Int : Component {
  int32_t val_{0};
};

class ArchetypeTests : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}

 private:
  SharedDescriptor desc_{
      SharedDescriptor::New(ArchetypeDescriptor::New<Int>({}))};
  ObservedPagePool page_pool_{ObservedPagePool::New()};
  Archetype archetype_{desc_.Clone(), page_pool_.NewObserver()};
};

};  // namespace
