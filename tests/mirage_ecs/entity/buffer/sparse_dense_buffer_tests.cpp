#include <gtest/gtest.h>

#include "mirage_ecs/entity/buffer/sparse_dense_buffer.hpp"

using namespace mirage::ecs;

TEST(DenseBufferTests, Construct) {
  DenseBuffer buffer;
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_EQ(buffer.capacity(), 0);

  buffer = DenseBuffer({2 * sizeof(SparseId), alignof(SparseId)});
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_EQ(buffer.capacity(), 2);
}

TEST(DenseBufferTests, PushAndAccess) {
  DenseBuffer buffer({2 * sizeof(SparseId), alignof(SparseId)});

  EXPECT_EQ(buffer.size(), 0);
  EXPECT_EQ(buffer.capacity(), 2);

  buffer.Push(0);
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_EQ(buffer[0], 0);

  buffer.Push(1);
  EXPECT_EQ(buffer.size(), 2);
  EXPECT_EQ(buffer[1], 1);
}

TEST(DenseBufferTests, RemoveTail) {
  DenseBuffer buffer({2 * sizeof(SparseId), alignof(SparseId)});

  buffer.Push(0);
  buffer.Push(1);
  EXPECT_EQ(buffer.size(), 2);

  buffer.RemoveTail();
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_EQ(buffer[0], 0);

  buffer.RemoveTail();
  EXPECT_EQ(buffer.size(), 0);
}

TEST(DenseBufferTests, Reserve) {
  DenseBuffer buffer({DenseBuffer::kUnitSize, alignof(SparseId)});
  buffer.Reserve(0);
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_EQ(buffer.capacity(), 1);

  buffer.Push(1);
  buffer.Reserve(2 * DenseBuffer::kUnitSize);
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_EQ(buffer.capacity(), 2);
  EXPECT_EQ(buffer[0], 1);
}

TEST(SparseBufferTests, Construct) {
  SparseBuffer buffer;
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_EQ(buffer.hole_cnt(), 0);
  EXPECT_EQ(buffer.capacity(), 0);

  const auto unit_size = sizeof(DenseId) + sizeof(uint16_t);
  buffer = SparseBuffer({2 * unit_size, alignof(DenseId)});
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_EQ(buffer.hole_cnt(), 2);
  EXPECT_EQ(buffer.capacity(), 2);
}

TEST(SparseBufferTests, FillAndAccess) {
  const auto unit_size = sizeof(DenseId) + sizeof(uint16_t);
  auto buffer = SparseBuffer({2 * unit_size, alignof(DenseId)});

  const auto dense_1 = 1;
  const auto index_0 = buffer.FillHole(dense_1);
  EXPECT_EQ(index_0, 0);
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_EQ(buffer.hole_cnt(), 1);
  EXPECT_EQ(buffer.capacity(), 2);
  EXPECT_EQ(buffer[index_0], dense_1);

  const auto dense_2 = 2;
  const auto index_1 = buffer.FillHole(dense_2);
  EXPECT_EQ(index_1, 1);
  EXPECT_EQ(buffer.size(), 2);
  EXPECT_EQ(buffer.hole_cnt(), 0);
  EXPECT_EQ(buffer[index_1], dense_2);
}

TEST(SparseBufferTests, Remove) {
  const auto unit_size = sizeof(DenseId) + sizeof(uint16_t);
  auto buffer = SparseBuffer({2 * unit_size, alignof(DenseId)});

  const auto index_0 = buffer.FillHole(1);
  const auto index_1 = buffer.FillHole(2);

  buffer.Remove(index_0);
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_EQ(buffer.hole_cnt(), 1);
  EXPECT_EQ(buffer.capacity(), 2);
  EXPECT_EQ(buffer[index_0], kInvalidDenseId);
  EXPECT_EQ(buffer[index_1], 2);

  buffer.Remove(index_1);
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_EQ(buffer.hole_cnt(), 2);
  EXPECT_EQ(buffer[index_1], kInvalidDenseId);

  // Last in first out
  const auto index_1_ = buffer.FillHole(3);
  EXPECT_EQ(index_1_, index_1);
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_EQ(buffer.hole_cnt(), 1);
  EXPECT_EQ(buffer[index_1_], 3);
}

TEST(SparseBufferTests, Reserve) {
  SparseBuffer buffer({SparseBuffer::kUnitSize, alignof(DenseId)});

  buffer.Reserve(0);
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_EQ(buffer.hole_cnt(), 1);
  EXPECT_EQ(buffer.capacity(), 1);

  const auto index_0 = buffer.FillHole(1);
  buffer.Reserve(2 * SparseBuffer::kUnitSize);
  EXPECT_EQ(buffer.size(), 1);
  EXPECT_EQ(buffer.hole_cnt(), 1);
  EXPECT_EQ(buffer.capacity(), 2);
  EXPECT_EQ(buffer[index_0], 1);

  const auto index_1 = buffer.FillHole(2);
  EXPECT_EQ(index_1, 1);
  EXPECT_EQ(buffer.size(), 2);
  EXPECT_EQ(buffer.hole_cnt(), 0);
  EXPECT_EQ(buffer.capacity(), 2);
  EXPECT_EQ(buffer[index_1], 2);
}
