#include <gtest/gtest.h>

#include <concepts>

#include "mirage_base/util/func_trait.hpp"

using namespace mirage::base;

int32_t Add(int32_t a, int32_t b) { return a + b; }

template <typename AddFuncTrait>
void TestAddFuncTrait(FuncType func_type) {
  using Arg0 = typename AddFuncTrait::ArgsTypeList::template Get<0>::Type;
  using Arg1 = typename AddFuncTrait::ArgsTypeList::template Get<1>::Type;

  constexpr bool ret = std::same_as<typename AddFuncTrait::ReturnType, int32_t>;
  constexpr size_t arg_size = AddFuncTrait::ArgsTypeList::size();
  constexpr bool arg0 = std::same_as<Arg0, int32_t>;
  constexpr bool arg1 = std::same_as<Arg1, int32_t>;

  EXPECT_TRUE(ret);
  EXPECT_EQ(arg_size, 2);
  EXPECT_TRUE(arg0);
  EXPECT_TRUE(arg1);
  EXPECT_EQ(AddFuncTrait::kFuncType, func_type);
}

TEST(FuncTraitTests, TestAddFuncTrait) {
  TestAddFuncTrait<FuncTrait<decltype(Add)>>(FuncType::kFunc);
  TestAddFuncTrait<FuncTrait<decltype(&Add)>>(FuncType::kFuncPtr);
  TestAddFuncTrait<FuncTrait<decltype(Add)&>>(FuncType::kFuncRef);
}
