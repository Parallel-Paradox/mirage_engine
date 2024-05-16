#include <gtest/gtest.h>

#include "mirage_framework/base/container/hash_map.hpp"
#include "mirage_framework/base/util/hash.hpp"

using namespace mirage;

namespace {

struct UnHash {};

struct ContextHash {
  size_t num_{0};

  bool operator==(const ContextHash& other) const { return num_ == other.num_; }
};

}  // namespace

template <>
struct Hash<ContextHash> {
  size_t context_{1};

  size_t operator()(const ContextHash& val) { return val.num_ + context_; }
};

TEST(HashMapTests, Hash) {
  EXPECT_TRUE(HashKeyType<size_t>);
  EXPECT_FALSE(HashKeyType<UnHash>);

  EXPECT_TRUE(HashKeyType<ContextHash>);
  Hash<ContextHash> hasher;
  EXPECT_EQ(hasher(ContextHash{0}), 1);
  EXPECT_EQ(hasher(ContextHash{1}), 2);
}
