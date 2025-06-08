#include <gtest/gtest.h>

#include <concepts>

#include "mirage_base/util/func_trait.hpp"

using namespace mirage::base;

namespace {

int32_t Add(const int32_t a, const int32_t b) { return a + b; }

}  // namespace

template <typename AddFuncTrait>
void TestAddFuncTrait(FuncType func_type) {
  using Arg0 = typename AddFuncTrait::ArgsTypeList::template Get<0>::Type;
  using Arg1 = typename AddFuncTrait::ArgsTypeList::template Get<1>::Type;

  constexpr bool ret = std::same_as<typename AddFuncTrait::ReturnType, int32_t>;
  constexpr size_t args_size = AddFuncTrait::ArgsTypeList::size();
  // Note: The const declaration of parameters that are passed by value will be
  // implicitly removed in its signature.
  constexpr bool arg0 = std::same_as<Arg0, int32_t>;
  constexpr bool arg1 = std::same_as<Arg1, int32_t>;

  EXPECT_TRUE(ret);
  EXPECT_EQ(args_size, 2);
  EXPECT_TRUE(arg0);
  EXPECT_TRUE(arg1);
  EXPECT_EQ(AddFuncTrait::kFuncType, func_type);
}

TEST(FuncTraitTests, TestAddFuncTrait) {
  EXPECT_EQ(2, Add(1, 1));  // Make compiler don't ignore this function.
  TestAddFuncTrait<FuncTrait<decltype(Add)>>(FuncType::kFunc);
  TestAddFuncTrait<FuncTrait<decltype(&Add)>>(FuncType::kFuncPtr);
  TestAddFuncTrait<FuncTrait<decltype(Add)&>>(FuncType::kFuncRef);
}
