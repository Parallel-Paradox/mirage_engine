#include <gtest/gtest.h>

#include <utility>

#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/entity/archetype.hpp"
#include "mirage_ecs/entity/generation_id.hpp"
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

    bundle_ = ComponentBundle();
    bundle_.Add(Bool{true});
    bundle_.Add(Int32{42});
    bundle_.Add(Int64{123456789});
  }

  void TearDown() override {}

  SharedDescriptor desc_;
  Archetype archetype_;
  ComponentBundle bundle_;
};

TEST_F(ArchetypeTests, PushWithBundle) {
  EntityId entity_id = {0, 0};
  const auto index = archetype_.Push(entity_id, bundle_);
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
  const auto index = archetype_.Push(entity_id, bundle_);

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

TEST_F(ArchetypeTests, IndexOperatorMutable) {
  const auto index = archetype_.Push(EntityId{0, 0}, bundle_);
  auto view = archetype_[index];

  // Modify component values
  view.Get<Bool>().value = false;
  view.Get<Int32>().value = 100;

  // Verify modifications
  auto modified_view = archetype_[index];
  EXPECT_EQ(modified_view.Get<Bool>().value, false);
  EXPECT_EQ(modified_view.Get<Int32>().value, 100);
}

TEST_F(ArchetypeTests, TakeNothing) {
  auto result = archetype_.TakeMany(desc_.Clone(), {});
  EXPECT_TRUE(result.empty());

  archetype_.Push(EntityId{0, 0}, bundle_);
  result = archetype_.TakeMany(desc_.Clone(), {});
  EXPECT_TRUE(result.empty());
}

TEST_F(ArchetypeTests, TakeMany) {
  Array<Archetype::Index> indices;
  for (uint32_t i = 0; i < 2048; ++i) {
    ComponentBundle bundle;
    bundle.Add(Bool{i % 2 == 0});
    bundle.Add(Int32{static_cast<int32_t>(i)});
    bundle.Add(Int64{i * 10});
    indices.Push(archetype_.Push(EntityId{i, 0}, bundle));
  }
  EXPECT_EQ(archetype_.size(), 2048);

  auto target_desc = ArchetypeDescriptor::New<Bool, Int32>({});
  auto shared_target = SharedDescriptor::New(std::move(target_desc));
  auto result =
      archetype_.TakeMany(std::move(shared_target), std::move(indices));
  EXPECT_EQ(archetype_.size(), 0);

  for (auto &buffer : result) {
    for (uint16_t i = 0; i < buffer.size(); ++i) {
      auto view = buffer[i];
      const auto iter = view.entity_id().index();
      EXPECT_EQ(view.entity_id().generation(), 0);
      EXPECT_EQ(view.Get<Bool>().value, iter % 2 == 0);
      EXPECT_EQ(view.Get<Int32>().value, static_cast<int32_t>(iter));
      EXPECT_EQ(view.TryGet<Int64>(), nullptr);
    }
  }

  EntityId entity_id = {0, 1};
  const auto index = archetype_.Push(entity_id, bundle_);
  EXPECT_EQ(archetype_.size(), 1);
  EXPECT_EQ(index, 0);

  auto view = archetype_[index];
  EXPECT_EQ(view.entity_id(), entity_id);
  EXPECT_EQ(view.Get<Bool>().value, true);
  EXPECT_EQ(view.Get<Int32>().value, 42);
  EXPECT_EQ(view.Get<Int64>().value, 123456789);
}
