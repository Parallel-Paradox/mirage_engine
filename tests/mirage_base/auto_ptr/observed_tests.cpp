#include <gtest/gtest.h>

#include "mirage_base/auto_ptr/observed.hpp"

using namespace mirage::base;

namespace {

struct Base {
  int32_t* base_destructed{nullptr};

  explicit Base(int32_t* base_destructed) : base_destructed(base_destructed) {}

  virtual ~Base() { *base_destructed += 1; }
};

struct Derive final : Base {
  int32_t* derive_destructed{nullptr};

  explicit Derive(int32_t* base_destructed, int32_t* derive_destructed)
      : Base(base_destructed), derive_destructed(derive_destructed) {}

  ~Derive() override { *derive_destructed += 1; }
};

}  // namespace

TEST(ObservedTests, LocalConstruct) {
  int32_t is_destructed = 0;

  // Default construct
  ObservedLocal<Base> observed;
  EXPECT_TRUE(observed.is_null());

  // List construct
  observed = ObservedLocal<Base>::New(&is_destructed);
  EXPECT_FALSE(observed.is_null());
  EXPECT_EQ(observed.observer_cnt(), 0);

  // Raw construct
  const auto base = new Base(&is_destructed);
  observed = ObservedLocal<Base>(base);
  EXPECT_EQ(observed.raw_ptr(), base);
  EXPECT_EQ(observed.observer_cnt(), 0);
  EXPECT_EQ(is_destructed, 1);  // List constructed observed is freed.

  // New observer
  auto observer = observed.NewObserver();
  EXPECT_EQ(observed.observer_cnt(), 1);
  EXPECT_EQ(observer.observer_cnt(), 1);
  EXPECT_EQ(observer.raw_ptr(), base);
  EXPECT_FALSE(observer.is_null());

  // Reset observer when observed is not null
  observer.Reset();
  EXPECT_EQ(observed.observer_cnt(), 0);
  EXPECT_TRUE(observer.is_null());
  EXPECT_FALSE(observed.is_null());

  // Reset observer when observed is null
  observer = observed.NewObserver();
  observed.Reset();
  EXPECT_EQ(observed.observer_cnt(), 0);
  EXPECT_EQ(observer.observer_cnt(), 1);
  EXPECT_TRUE(observed.is_null());
  EXPECT_TRUE(observer.is_null());
  EXPECT_EQ(is_destructed, 2);  // Observed is freed.
}

TEST(ObservedTests, LocalPtrOps) {
  int32_t cnt = 0;
  const auto ptr = ObservedLocal<Base>::New(&cnt);
  EXPECT_EQ(*ptr->base_destructed, 0);

  *((*ptr).base_destructed) = 1;  // NOLINT: Test deref.
  EXPECT_EQ(*ptr->base_destructed, 1);

  const auto observer = ptr.NewObserver();
  EXPECT_EQ(*(observer->base_destructed), 1);

  *(*observer).base_destructed = 2;  // NOLINT: Test deref.
  EXPECT_EQ(*ptr->base_destructed, 2);
}

TEST(ObservedTests, LocalCloneObserver) {
  int32_t is_destructed = 0;

  auto observed = ObservedLocal<Base>::New(&is_destructed);
  EXPECT_EQ(observed.observer_cnt(), 0);
  EXPECT_EQ(is_destructed, 0);

  const auto observer1 = observed.NewObserver();
  EXPECT_EQ(observed.observer_cnt(), 1);
  EXPECT_EQ(observer1.observer_cnt(), 1);

  const auto observer2 = observer1.Clone();
  EXPECT_EQ(observed.observer_cnt(), 2);
  EXPECT_EQ(observer1.observer_cnt(), 2);

  observed.Reset();
  const auto observer3 = observer2.Clone();
  EXPECT_EQ(observer2.observer_cnt(), 3);
  EXPECT_TRUE(observer2.is_null());
  EXPECT_EQ(observer3.observer_cnt(), 3);
  EXPECT_TRUE(observer3.is_null());
}

TEST(ObservedTests, LocalConvertDeriveToBase) {
  int32_t base_destructed = 0;
  int32_t derive_destructed = 0;

  // Convert from derive to base is always successful.
  auto derive =
      ObservedLocal<Derive>::New(&base_destructed, &derive_destructed);
  auto derive_obs = derive.NewObserver();

  ObservedLocal<Base> base = std::move(derive).Convert<Base>();
  const LocalObserver<Base> base_obs = std::move(derive_obs).Convert<Base>();

  EXPECT_TRUE(derive.is_null());  // NOLINT(*-use-after-move): Allow for test.
  EXPECT_TRUE(
      derive_obs.is_null());  // NOLINT(*-use-after-move): Allow for test.
  EXPECT_FALSE(base.is_null());
  EXPECT_FALSE(base_obs.is_null());
  base.Reset();

  // Even holder is base, derive destructor is still called.
  EXPECT_EQ(base_destructed, 1);
  EXPECT_EQ(derive_destructed, 1);
  EXPECT_TRUE(base_obs.is_null());
}

TEST(ObservedTests, LocalConvertBaseToDerive) {
  // Can't convert from base to derive when base is the origin type.
  int32_t base_destructed = 0;
  auto base = ObservedLocal<Base>::New(&base_destructed);
  auto base_obs = base.NewObserver();
  auto derive = base.TryConvert<Derive>();
  auto derive_obs = base_obs.TryConvert<Derive>();
  EXPECT_TRUE(derive.is_null());
  EXPECT_TRUE(derive_obs.is_null());
  EXPECT_FALSE(base.is_null());
  EXPECT_FALSE(base_obs.is_null());
  EXPECT_EQ(base_destructed, 0);
  base.Reset();
  base_obs.Reset();
  base_destructed = 0;

  // Convert from base to derive when derive is the origin type.
  int32_t derive_destructed = 0;
  derive = ObservedLocal<Derive>::New(&base_destructed, &derive_destructed);
  derive_obs = derive.NewObserver();
  base = derive.TryConvert<Base>();
  base_obs = derive_obs.TryConvert<Base>();
  derive = base.TryConvert<Derive>();
  derive_obs = base_obs.TryConvert<Derive>();
  EXPECT_FALSE(derive.is_null());
  EXPECT_FALSE(derive_obs.is_null());
  EXPECT_TRUE(base.is_null());
  EXPECT_TRUE(base_obs.is_null());
  EXPECT_EQ(base_destructed, 0);
  EXPECT_EQ(derive_destructed, 0);
}
