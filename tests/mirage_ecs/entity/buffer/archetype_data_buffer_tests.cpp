#include <gtest/gtest.h>

#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"
#include "mirage_ecs/entity/buffer/archetype_data_buffer.hpp"
#include "mirage_ecs/entity/entity_id.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

using SharedDescriptor = SharedLocal<ArchetypeDescriptor>;

namespace {

struct Counter : Component {
  int32_t *destruct_cnt_{nullptr};

  Counter() = default;
  Counter(int32_t *destruct_cnt) : destruct_cnt_(destruct_cnt) {}

  Counter(Counter &&other) noexcept : destruct_cnt_(other.destruct_cnt_) {
    other.destruct_cnt_ = nullptr;
  }

  ~Counter() {
    if (destruct_cnt_) {
      *destruct_cnt_ += 1;
    }
    destruct_cnt_ = nullptr;
  }
};

class ArchetypeDataBufferTests : public ::testing::Test {
 protected:
  void TearDown() override {
    buffer_.Clear();
    destruct_cnt_ = 0;
  }

  SharedDescriptor desc_{
      SharedDescriptor::New(ArchetypeDescriptor::New<Counter>({}))};
  ArchetypeDataBuffer buffer_{
      {2 * (desc_->size() + sizeof(EntityId)), desc_->align()}, desc_.Clone()};
  int32_t destruct_cnt_{0};
};

}  // namespace

TEST_F(ArchetypeDataBufferTests, Construct) {
  ArchetypeDataBuffer buffer;
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_EQ(buffer.capacity(), 0);

  EXPECT_EQ(buffer_.size(), 0);
  EXPECT_EQ(buffer_.capacity(), 2);
  EXPECT_EQ(buffer_.is_full(), false);
  EXPECT_EQ(buffer_.unit_size(), desc_->size() + sizeof(EntityId));
}

TEST_F(ArchetypeDataBufferTests, PushBundleAndAccess) {
  ComponentBundle bundle;
  bundle.Add(Counter(&destruct_cnt_));

  const auto id_1 = EntityId{1, 0};
  buffer_.Push(id_1, bundle);
  EXPECT_EQ(buffer_.size(), 1);

  EXPECT_EQ(buffer_[0].entity_id(), id_1);
  EXPECT_EQ(buffer_[0].Get<Counter>().destruct_cnt_, &destruct_cnt_);
  EXPECT_EQ(destruct_cnt_, 0);

  const auto id_2 = EntityId{2, 0};
  bundle.Add(Counter(&destruct_cnt_));
  buffer_.Push(id_2, bundle);
  EXPECT_EQ(buffer_.size(), 2);
  EXPECT_EQ(buffer_[1].entity_id(), id_2);
  EXPECT_EQ(buffer_[1].Get<Counter>().destruct_cnt_, &destruct_cnt_);
  EXPECT_EQ(destruct_cnt_, 0);
}

TEST_F(ArchetypeDataBufferTests, PushViewAndClear) {
  auto buffer = ArchetypeDataBuffer(
      {2 * (desc_->size() + sizeof(EntityId)), desc_->align()}, desc_.Clone());
  ComponentBundle bundle;
  bundle.Add(Counter(&destruct_cnt_));
  const auto id_1 = EntityId{1, 0};
  buffer.Push(id_1, bundle);

  bundle.Add(Counter(&destruct_cnt_));
  buffer_.Push(buffer[0]);
  EXPECT_EQ(buffer_.size(), 1);
  EXPECT_EQ(buffer_[0].entity_id(), id_1);
  EXPECT_EQ(buffer_[0].Get<Counter>().destruct_cnt_, &destruct_cnt_);
  EXPECT_EQ(destruct_cnt_, 0);

  buffer.Clear();
  EXPECT_EQ(destruct_cnt_, 0);
  buffer_.Clear();
  EXPECT_EQ(buffer_.size(), 0);
  EXPECT_EQ(destruct_cnt_, 1);
}

TEST_F(ArchetypeDataBufferTests, RemoveTail) {
  ComponentBundle bundle;

  const auto id_1 = EntityId{1, 0};
  bundle.Add(Counter(&destruct_cnt_));
  buffer_.Push(id_1, bundle);

  const auto id_2 = EntityId{2, 0};
  bundle.Add(Counter(&destruct_cnt_));
  buffer_.Push(id_2, bundle);
  EXPECT_EQ(buffer_.size(), 2);

  buffer_.RemoveTail();
  EXPECT_EQ(buffer_.size(), 1);
  EXPECT_EQ(buffer_[0].entity_id(), id_1);
  EXPECT_EQ(destruct_cnt_, 1);

  buffer_.RemoveTail();
  EXPECT_EQ(buffer_.size(), 0);
  EXPECT_EQ(destruct_cnt_, 2);

  const auto id_3 = EntityId{3, 0};
  bundle.Add(Counter(&destruct_cnt_));
  buffer_.Push(id_3, bundle);
  EXPECT_EQ(buffer_.size(), 1);
  EXPECT_EQ(buffer_[0].entity_id(), id_3);
  EXPECT_EQ(destruct_cnt_, 2);
}

TEST_F(ArchetypeDataBufferTests, Reserve) {
  auto buffer = ArchetypeDataBuffer(
      {desc_->size() + sizeof(EntityId), desc_->align()}, desc_.Clone());
  ComponentBundle bundle;
  bundle.Add(Counter(&destruct_cnt_));
  buffer.Push({1, 0}, bundle);
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_EQ(buffer.capacity(), 1);
  EXPECT_EQ(buffer[0].Get<Counter>().destruct_cnt_, &destruct_cnt_);

  buffer.Reserve(0);
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_EQ(buffer.capacity(), 1);
  EXPECT_EQ(buffer[0].Get<Counter>().destruct_cnt_, &destruct_cnt_);

  buffer.Reserve(2);
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_EQ(buffer.capacity(), 2);
  EXPECT_EQ(buffer[0].Get<Counter>().destruct_cnt_, &destruct_cnt_);
  EXPECT_EQ(destruct_cnt_, 0);
}

TEST_F(ArchetypeDataBufferTests, TakeBuffer) {
  auto buffer = ArchetypeDataBuffer(
      {desc_->size() + sizeof(EntityId), desc_->align()}, desc_.Clone());
  ComponentBundle bundle;
  bundle.Add(Counter(&destruct_cnt_));
  buffer.Push({1, 0}, bundle);
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_EQ(destruct_cnt_, 0);

  auto taken_buffer = std::move(buffer).TakeBuffer();
  EXPECT_EQ(taken_buffer.size(), desc_->size() + sizeof(EntityId));
  EXPECT_EQ(taken_buffer.align(), desc_->align());
  EXPECT_EQ(destruct_cnt_, 1);
}
