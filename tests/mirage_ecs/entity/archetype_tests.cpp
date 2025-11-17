#include <gtest/gtest.h>

#include <utility>

#include "mirage_ecs/entity/archetype.hpp"
#include "mirage_ecs/entity/entity_id.hpp"
#include "mirage_ecs/util/marker.hpp"

using namespace mirage::ecs;
using namespace mirage::base;

namespace {

using SharedDescriptor = SharedLocal<ArchetypeDescriptor>;

struct Bool {
  MIRAGE_COMPONENT;
  bool value{false};
};

struct Int32 {
  MIRAGE_COMPONENT;
  int32_t value{0};
};

struct Int64 {
  MIRAGE_COMPONENT;
  int64_t value{0};
};

}  // namespace

class ArchetypeTests : public ::testing::Test {
 protected:
  void SetUp() override {
    auto desc = ArchetypeDescriptor::New<Bool, Int64, Int32>({});
    desc_ = SharedDescriptor::New(std::move(desc));
    archetype_ = Archetype(desc_.Clone());
  }

  void TearDown() override {}

  SharedDescriptor desc_;
  Archetype archetype_;
};

TEST_F(ArchetypeTests, PushWithBundle) {
  EntityId entity_id = {0, 0};
  ComponentBundle bundle;
  bundle.Add(Bool{true});
  bundle.Add(Int32{42});
  bundle.Add(Int64{123456789});

  const auto index = archetype_.Push(entity_id, bundle);
  EXPECT_EQ(archetype_.size(), 1);
  EXPECT_EQ(index, 0);

  auto view = archetype_[index];
  EXPECT_EQ(view.entity_id(), entity_id);
  EXPECT_EQ(view.Get<Bool>().value, true);
  EXPECT_EQ(view.Get<Int32>().value, 42);
  EXPECT_EQ(view.Get<Int64>().value, 123456789);
}

TEST_F(ArchetypeTests, PushWithView) {
  EntityId entity_id = {0, 0};
  ComponentBundle bundle;
  bundle.Add(Bool{true});
  bundle.Add(Int32{42});
  bundle.Add(Int64{123456789});

  const auto index = archetype_.Push(entity_id, bundle);

  auto buffer = archetype_.TakeMany(desc_.Clone(), {index});
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_EQ(archetype_.size(), 0);

  auto view = buffer[0][0];
  EXPECT_EQ(view.entity_id(), entity_id);
  EXPECT_EQ(view.Get<Bool>().value, true);
  EXPECT_EQ(view.Get<Int32>().value, 42);
  EXPECT_EQ(view.Get<Int64>().value, 123456789);

  const auto index_1 = archetype_.Push(std::move(view));
  EXPECT_EQ(archetype_.size(), 1);
  EXPECT_EQ(index_1, 0);

  auto view_1 = archetype_[index_1];
  EXPECT_EQ(view_1.entity_id(), entity_id);
  EXPECT_EQ(view_1.Get<Bool>().value, true);
  EXPECT_EQ(view_1.Get<Int32>().value, 42);
  EXPECT_EQ(view_1.Get<Int64>().value, 123456789);
}
