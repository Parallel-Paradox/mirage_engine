#include <gtest/gtest.h>
#include <sys/_types/_int32_t.h>

#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/entity/archetype_data_page.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"
#include "mirage_ecs/entity/entity_id.hpp"
#include "mirage_ecs/util/marker.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

using SharedDescriptor = ArchetypeDataPage::SharedDescriptor;

namespace {

struct Counter : Component {
  int32_t id_{-1};
  int32_t *destruct_cnt_{nullptr};

  Counter() = default;
  Counter(const int32_t id, int32_t *destruct_cnt)
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
      SharedDescriptor::New(ArchetypeDescriptor::New<Counter>({}))};
  ArchetypeDataPage page_{desc_->size() * 2, desc_->align()};
  int32_t destruct_cnt_{0};
};

}  // namespace

TEST_F(ArchetypeDataPageTests, Initialize) {
  auto page = ArchetypeDataPage(desc_->size() * 2, desc_->align());
  EXPECT_FALSE(page.is_initialized());
  EXPECT_EQ(page.capacity(), 0);
  EXPECT_EQ(page.size(), 0);
  EXPECT_NE(page.buffer().ptr(), nullptr);
  EXPECT_EQ(page.buffer().size(), desc_->size() * 2);
  const auto buffer_address =
      (size_t)page.buffer().ptr();  // NOLINT: Check align
  EXPECT_EQ(buffer_address % page.buffer().align(), 0);

  page.Initialize(desc_.Clone());
  EXPECT_TRUE(page.is_initialized());
  EXPECT_EQ(page.capacity(), page.buffer().size() / page.descriptor()->size());
  EXPECT_EQ(page.size(), 0);
}

TEST_F(ArchetypeDataPageTests, Curd) {
  auto bundle = ComponentBundle();

  // Push to empty page
  bundle.Add(Counter(0, &destruct_cnt_));
  int32_t rv = page_.Push({0, 0}, bundle);
  EXPECT_EQ(rv, 0);
  EXPECT_EQ(page_.size(), 1);
  EXPECT_EQ(destruct_cnt_, 0);

  // Push to non-empty page
  bundle.Add(Counter(1, &destruct_cnt_));
  rv = page_.Push({1, 0}, bundle);
  EXPECT_EQ(rv, 1);
  EXPECT_EQ(page_.size(), 2);
  EXPECT_EQ(destruct_cnt_, 0);

  // Push to full page
  bundle.Add(Counter(2, &destruct_cnt_));
  rv = page_.Push({2, 0}, bundle);
  EXPECT_EQ(rv, -1);
  EXPECT_EQ(page_.size(), 2);
  EXPECT_EQ(destruct_cnt_, 0);
  EXPECT_EQ(bundle.size(), 1);

  // View page contents
  EXPECT_EQ(page_[0].Get<Counter>().id_, 0);
  EXPECT_EQ(page_[1].Get<Counter>().id_, 1);

  auto expect_entity_id = EntityId{0, 0};
  EXPECT_EQ(page_[0].entity_id(), expect_entity_id);
  expect_entity_id = EntityId{1, 0};
  EXPECT_EQ(page_[1].entity_id(), expect_entity_id);

  // Take many from page
  auto courier = page_.TakeMany({0, 1});
  EXPECT_EQ(courier.size(), 2);
  EXPECT_EQ(page_.size(), 0);
  EXPECT_EQ(destruct_cnt_, 0);

  // Push many to page
  for (auto view : courier) {
    rv = page_.Push(view);
    EXPECT_TRUE(rv >= 0);
  }
  EXPECT_EQ(page_.size(), 2);
  EXPECT_EQ(destruct_cnt_, 0);

  // Clear page
  page_.Clear();
  EXPECT_EQ(page_.size(), 0);
  EXPECT_EQ(destruct_cnt_, 2);
}

TEST_F(ArchetypeDataPageTests, SparseDense) {
  Array<int32_t> expect_sparse = {-1, -1};
  Array<int32_t> expect_dense = {};
  Array<int32_t> expect_hole = {1, 0};
  EXPECT_EQ(page_.sparse(), expect_sparse);
  EXPECT_EQ(page_.dense(), expect_dense);
  EXPECT_EQ(page_.hole(), expect_hole);

  auto bundle = ComponentBundle();

  bundle.Add(Counter(0, &destruct_cnt_));
  page_.Push({0, 0}, bundle);

  bundle.Add(Counter(1, &destruct_cnt_));
  page_.Push({1, 0}, bundle);

  page_.Remove(0);

  expect_sparse = {-1, 0};
  expect_dense = {1};
  expect_hole = {0};
  EXPECT_EQ(page_.size(), 1);
  EXPECT_EQ(destruct_cnt_, 1);
  EXPECT_EQ(page_[1].Get<Counter>().id_, 1);
  EXPECT_EQ(page_.sparse(), expect_sparse);
  EXPECT_EQ(page_.dense(), expect_dense);
  EXPECT_EQ(page_.hole(), expect_hole);

  bundle.Add(Counter(2, &destruct_cnt_));
  int32_t rv = page_.Push({2, 0}, bundle);
  EXPECT_EQ(rv, 0);
  expect_sparse = {1, 0};
  expect_dense = {1, 0};
  expect_hole = {};
  EXPECT_EQ(page_.size(), 2);
  EXPECT_EQ(destruct_cnt_, 1);
  EXPECT_EQ(page_[rv].Get<Counter>().id_, 2);
  EXPECT_EQ(page_.sparse(), expect_sparse);
  EXPECT_EQ(page_.dense(), expect_dense);
  EXPECT_EQ(page_.hole(), expect_hole);

  page_.RemoveMany({0, 1});
  expect_sparse = {-1, -1};
  expect_dense = {};
  expect_hole = {0, 1};
  EXPECT_EQ(page_.size(), 0);
  EXPECT_EQ(destruct_cnt_, 3);
  EXPECT_EQ(page_.sparse(), expect_sparse);
  EXPECT_EQ(page_.dense(), expect_dense);
  EXPECT_EQ(page_.hole(), expect_hole);
}

TEST_F(ArchetypeDataPageTests, Reserve) {
  auto page = ArchetypeDataPage(desc_->size() * 2, desc_->align());
  EXPECT_FALSE(page.is_initialized());

  page.Reserve(desc_->size());
  EXPECT_EQ(page.buffer().size(), desc_->size() * 2);

  page.Reserve(desc_->size() * 3);
  EXPECT_EQ(page.buffer().size(), desc_->size() * 3);

  page.Initialize(desc_.Clone());
  EXPECT_TRUE(page.is_initialized());

  auto bundle = ComponentBundle();
  bundle.Add(Counter(0, &destruct_cnt_));
  page.Push({0, 0}, bundle);

  Array<int32_t> expect_sparse = {0, -1, -1};
  Array<int32_t> expect_dense = {0};
  Array<int32_t> expect_hole = {2, 1};
  EXPECT_EQ(page.sparse(), expect_sparse);
  EXPECT_EQ(page.dense(), expect_dense);
  EXPECT_EQ(page.hole(), expect_hole);

  page.Reserve(desc_->size() * 5);
  EXPECT_EQ(page.buffer().size(), desc_->size() * 5);
  EXPECT_EQ(page.size(), 1);
  EXPECT_EQ(destruct_cnt_, 0);
  EXPECT_EQ(page[0].Get<Counter>().id_, 0);

  expect_sparse = {0, -1, -1, -1, -1};
  expect_dense = {0};
  expect_hole = {2, 1, 4, 3};
  EXPECT_EQ(page.sparse(), expect_sparse);
  EXPECT_EQ(page.dense(), expect_dense);
  EXPECT_EQ(page.hole(), expect_hole);
}
