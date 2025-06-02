#ifndef MIRAGE_ECS_SYSTEM_SYSTEM
#define MIRAGE_ECS_SYSTEM_SYSTEM

#include <concepts>
#include <functional>

#include "mirage_base/auto_ptr/owned.hpp"
#include "mirage_base/util/func_trait.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/system/extract.hpp"
#include "mirage_ecs/system/system_context.hpp"

namespace mirage::ecs {

template <IsExtractable T>
consteval bool CheckIsExtractable() {
  return IsExtractable<T>;
};

template <typename... Args>
consteval bool IsArgsExtractable(base::TypeList<Args...>) {
  if constexpr (sizeof...(Args) == 0) {
    return true;
  } else {
    return (CheckIsExtractable<Args>(), ...);
  }
}

// TODO: Coroutine system
template <typename Func>
concept IsSystem = std::same_as<base::FuncReturnType<Func>, void> &&
                   IsArgsExtractable(base::FuncArgsTypeList<Func>());

class System {
 public:
  using SystemFunc =
      std::function<void(World&, base::Owned<SystemContext>& context)>;

  System() = delete;
  MIRAGE_ECS ~System() = default;

  System(const System&) = delete;
  System& operator=(const System&) = delete;

  MIRAGE_ECS System(System&&) = default;
  MIRAGE_ECS System& operator=(System&&) = default;

  template <typename Func>
    requires IsSystem<Func>
  static System From(Func func, base::Owned<SystemContext>&& context =
                                    base::Owned<SystemContext>::New()) {
    using ArgsTypeList = base::FuncArgsTypeList<Func>;
    constexpr size_t kParamsCount = ArgsTypeList::size();
    return System(EraseFuncSignature(std::move(func),
                                     std::make_index_sequence<kParamsCount>{}),
                  std::move(context));
  }

  MIRAGE_ECS void Run(World& world);

 private:
  MIRAGE_ECS System(SystemFunc&& system_func,
                    base::Owned<SystemContext>&& context);

  template <typename Func, size_t... Index>
    requires IsSystem<Func>
  static SystemFunc EraseFuncSignature(Func func,
                                       std::index_sequence<Index...>) {
    using ArgsTypeList = base::FuncArgsTypeList<Func>;
    return [func = std::move(func)](
               [[maybe_unused]] World& world,
               [[maybe_unused]] base::Owned<SystemContext>& context) {
      func(Extract<base::GetTypeFromList<ArgsTypeList, Index>>::From(
          world, context)...);
    };
  }

  SystemFunc system_func_;
  base::Owned<SystemContext> context_;
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SYSTEM_SYSTEM
