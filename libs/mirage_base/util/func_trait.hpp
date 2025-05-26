#ifndef MIRAGE_BASE_UTIL_FUNC_TRAIT
#define MIRAGE_BASE_UTIL_FUNC_TRAIT

#include <utility>

#include "mirage_base/util/type_list.hpp"

namespace mirage::base {

enum FuncType {
  kInvalid,
  kFunc,
  kFuncPtr,
  kFuncRef,
};

struct FuncTraitBase {
  using ReturnType = void;
  using ArgsTypeList = TypeList<>;
  constexpr static auto kFuncType = FuncType::kInvalid;
};

template <typename Func>
struct FuncTrait : FuncTraitBase {};

template <typename Ret, typename... Args>
struct FuncTrait<Ret(Args...)> : FuncTraitBase {
  using ReturnType = Ret;
  using ArgsTypeList = TypeList<Args...>;
  constexpr static auto kFuncType = FuncType::kFunc;
};

template <typename Ret, typename... Args>
struct FuncTrait<Ret (*)(Args...)> : FuncTrait<Ret(Args...)> {
  constexpr static auto kFuncType = FuncType::kFuncPtr;
};

template <typename Ret, typename... Args>
struct FuncTrait<Ret (&)(Args...)> : FuncTrait<Ret(Args...)> {
  constexpr static auto kFuncType = FuncType::kFuncRef;
};

template <typename Func>
using FuncReturnType = typename FuncTrait<Func>::ReturnType;

template <typename Func>
using FuncArgsTypeList = typename FuncTrait<Func>::ArgsTypeList;

}  // namespace mirage::base

#endif
