#ifndef MIRAGE_ECS_SYSTEM_SYSTEM
#define MIRAGE_ECS_SYSTEM_SYSTEM

#include <concepts>
#include <functional>

#include "mirage_base/util/func_trait.hpp"
#include "mirage_ecs/define.hpp"
#include "mirage_ecs/system/context.hpp"
#include "mirage_ecs/system/extract.hpp"

namespace mirage::ecs {

consteval bool IsArgsExtractable(base::TypeList<>) { return true; }

template <typename Arg0, typename... Args>
  requires IsExtractable<Arg0>
consteval bool IsArgsExtractable(base::TypeList<Arg0, Args...>) {
  return IsArgsExtractable(base::TypeList<Args...>());
}

template <typename Func>
concept IsSystem = std::same_as<base::FuncReturnType<Func>, void> &&
                   IsArgsExtractable(base::FuncArgsTypeList<Func>());

class System {
 public:
  MIRAGE_ECS System() = default;
  MIRAGE_ECS ~System() = default;

  MIRAGE_ECS System(const System&) = delete;
  MIRAGE_ECS System& operator=(const System&) = delete;

  MIRAGE_ECS System(System&&) = default;
  MIRAGE_ECS System& operator=(System&&) = default;

  template <typename Func>
    requires IsSystem<Func>
  static System From(Func func) {
    using ArgsTypeList = base::FuncArgsTypeList<Func>;
    constexpr size_t kParamsCount = ArgsTypeList::size();

    System system;
    system.set_system_func(std::move(func),
                           std::make_index_sequence<kParamsCount>{});
    return system;
  }

 private:
  template <typename Func, size_t... Index>
    requires IsSystem<Func>
  void set_system_func(Func func, std::index_sequence<Index...>) {
    using ArgsTypeList = base::FuncArgsTypeList<Func>;
    system_func_ = [func = std::move(func)](Context& context) {
      func(Extract<base::GetTypeFromList<ArgsTypeList, Index>>::From(
          context)...);
    };
  }

  std::function<void(Context& context)> system_func_;
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SYSTEM_SYSTEM
