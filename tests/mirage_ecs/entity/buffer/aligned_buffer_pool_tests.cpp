#include <gtest/gtest.h>

#include "mirage_ecs/entity/buffer/aligned_buffer.hpp"
#include "mirage_ecs/entity/buffer/aligned_buffer_pool.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

TEST(AlignedBufferPoolTests, Allocate) {
  AlignedBufferPool pool;

  auto buffer = pool.Allocate(2);
  EXPECT_EQ(buffer.size(), AlignedBufferPool::kBufferSize);
  EXPECT_EQ(buffer.align(), 8);

  buffer = pool.Allocate(16);
  EXPECT_EQ(buffer.size(), AlignedBufferPool::kBufferSize);
  EXPECT_EQ(buffer.align(), 16);
}

TEST(AlignedBufferPoolTests, Reallocate) {
  AlignedBufferPool pool;

  auto buffer = pool.Allocate(4);
  void* ptr = buffer.ptr();
  pool.Release(std::move(buffer));

  buffer = pool.Allocate(8);
  EXPECT_EQ(ptr, buffer.ptr());
  pool.Release(std::move(buffer));

  buffer = pool.Allocate(16);
  EXPECT_NE(ptr, buffer.ptr());
}

TEST(AlignedBufferPoolTests, BigAlignReallocate) {
  AlignedBufferPool pool;

  auto buffer = pool.Allocate(16);
  void* ptr = buffer.ptr();
  pool.Release(std::move(buffer));

  buffer = pool.Allocate(8);
  EXPECT_EQ(ptr, buffer.ptr());
  EXPECT_EQ(buffer.align(), 16);
}
