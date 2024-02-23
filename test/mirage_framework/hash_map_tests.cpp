#include <gtest/gtest.h>

#include "mirage_framework/base/util/hash.hpp"

using namespace mirage;

struct UnHash {};

struct ContextHash {};

template <>
struct Hash<ContextHash> {
  size_t context_{0};

  size_t operator()(const ContextHash& val) {
    size_t ret = context_;
    context_ += 1;
    return ret;
  }
};

TEST(HashMapTests, Hash) {
  EXPECT_TRUE(HashKeyType<size_t>);
  EXPECT_FALSE(HashKeyType<UnHash>);

  EXPECT_TRUE(HashKeyType<ContextHash>);
  Hash<ContextHash> hasher;
  EXPECT_EQ(hasher(ContextHash()), 0);
  EXPECT_EQ(hasher(ContextHash()), 1);
}
