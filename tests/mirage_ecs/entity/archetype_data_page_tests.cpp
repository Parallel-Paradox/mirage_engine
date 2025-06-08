#include <gtest/gtest.h>

#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/entity/archetype_data_page.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"
#include "mirage_ecs/util/marker.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

using SharedDescriptor = ArchetypeDataPage::SharedDescriptor;

namespace {

struct Counter : Component {
  int32_t id_{-1};
  int32_t *destruct_cnt_{nullptr};

  Counter() = default;
  Counter(int32_t id, int32_t *destruct_cnt)
      : id_(id), destruct_cnt_(destruct_cnt) {}

  Counter(Counter &&other) noexcept
      : id_(other.id_), destruct_cnt_(other.destruct_cnt_) {
    other.id_ = -1;
    other.destruct_cnt_ = nullptr;
  }

  ~Counter() {
    if (destruct_cnt_) {
      *destruct_cnt_ += 1;
    }
    id_ = -1;
    destruct_cnt_ = nullptr;
  }
};

class ArchetypeDataPageTests : public ::testing::Test {
 protected:
  void SetUp() override { page_.Initialize(desc_.Clone()); }

  void TearDown() override {
    destruct_cnt_ = 0;
    page_.Reset();
  }

  SharedDescriptor desc_{
      SharedDescriptor::New(ArchetypeDescriptor::New<Counter>())};
  ArchetypeDataPage page_{desc_->size() * 2, desc_->align()};
  int32_t destruct_cnt_{0};
};

}  // namespace

TEST_F(ArchetypeDataPageTests, Initialize) {
  auto desc = SharedDescriptor::New(ArchetypeDescriptor::New<Counter>());
  auto page = ArchetypeDataPage(16, desc->align());
  EXPECT_FALSE(page.is_initialized());
  EXPECT_EQ(page.capacity(), 0);
  EXPECT_EQ(page.size(), 0);
  EXPECT_NE(page.buffer().ptr(), nullptr);
  EXPECT_EQ(page.buffer().size(), 16);
  size_t buffer_address = (size_t)page.buffer().ptr();  // NOLINT: Check align
  EXPECT_EQ(buffer_address % page.buffer().align(), 0);

  page.Initialize(std::move(desc));
  EXPECT_TRUE(page.is_initialized());
  EXPECT_EQ(page.capacity(), page.buffer().size() / page.descriptor()->size());
  EXPECT_EQ(page.size(), 0);
}

TEST_F(ArchetypeDataPageTests, Push) {
  auto bundle = ComponentBundle();

  bundle.Add(Counter(0, &destruct_cnt_));
  bool rv = page_.Push(bundle);
  EXPECT_TRUE(rv);
  EXPECT_EQ(page_.size(), 1);
  EXPECT_EQ(destruct_cnt_, 0);

  bundle.Add(Counter(1, &destruct_cnt_));
  rv = page_.Push(bundle);
  EXPECT_TRUE(rv);
  EXPECT_EQ(page_.size(), 2);
  EXPECT_EQ(destruct_cnt_, 0);

  EXPECT_EQ(page_[0].Get<Counter>().id_, 0);
  EXPECT_EQ(page_[1].Get<Counter>().id_, 1);
}

TEST_F(ArchetypeDataPageTests, Pop) {}

TEST_F(ArchetypeDataPageTests, Remove) {
  // TODO
}

TEST_F(ArchetypeDataPageTests, Slice) {
  // TODO
}
