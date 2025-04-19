#ifndef MIRAGE_BASE_UTIL_FUNC_TRAIT
#define MIRAGE_BASE_UTIL_FUNC_TRAIT

#include <utility>

#include "mirage_base/util/type_list.hpp"

namespace mirage::base {

enum class FuncType {
  kInvalid,
  kFunc,
  kFuncPtr,
  kFuncRef,
};

struct FuncTraitBase {
  using ReturnType = void;
  using ArgsTypeList = TypeList<>;
  constexpr static FuncType kFuncType = FuncType::kInvalid;
};

template <class Func>
struct FuncTrait : FuncTraitBase {};

template <class Ret, class... Args>
struct FuncTrait<Ret(Args...)> : FuncTraitBase {
  using ReturnType = Ret;
  using ArgsTypeList = TypeList<Args...>;
  constexpr static FuncType kFuncType = FuncType::kFunc;
};

template <class Ret, class... Args>
struct FuncTrait<Ret (*)(Args...)> : FuncTrait<Ret(Args...)> {
  constexpr static FuncType kFuncType = FuncType::kFuncPtr;
};

template <class Ret, class... Args>
struct FuncTrait<Ret (&)(Args...)> : FuncTrait<Ret(Args...)> {
  constexpr static FuncType kFuncType = FuncType::kFuncRef;
};

}  // namespace mirage::base

#endif
