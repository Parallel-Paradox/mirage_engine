#ifndef MIRAGE_ECS_SYSTEM_RESOURCE
#define MIRAGE_ECS_SYSTEM_RESOURCE

#include <type_traits>

#include "mirage_base/auto_ptr/owned.hpp"
#include "mirage_base/define/check.hpp"
#include "mirage_ecs/framework/world.hpp"
#include "mirage_ecs/system/extract.hpp"
#include "mirage_ecs/system/system_context.hpp"
#include "mirage_ecs/util/marker.hpp"

namespace mirage::ecs {

struct SystemArgs_Res {};

template <typename T>
concept IsSystemArgs_Res = std::derived_from<T, SystemArgs_Res>;

template <typename T>
  requires IsResource<T>
class Res : SystemArgs_Res {
 public:
  Res() = default;
  ~Res() = default;

  explicit Res(T* raw_ptr) : raw_ptr_(raw_ptr) {}

  T& operator*() const { return *raw_ptr_; }
  T* operator->() const { return raw_ptr_; }

  [[nodiscard]] T* raw_ptr() const { return raw_ptr_; }

 private:
  T* raw_ptr_;
};

template <typename T>
  requires IsResource<T>
struct Extract<Res<T>> {
  static Res<T> From(World& world,
                     [[maybe_unused]] base::Owned<SystemContext>& context) {
    T* raw_ptr = nullptr;
    if constexpr (std::is_const_v<T>) {
      raw_ptr = world.TryGetResource<std::remove_const<T>>();
    } else {
      raw_ptr = world.TryGetResource<T>();
    }
    MIRAGE_DCHECK(raw_ptr);
    return Res<T>(raw_ptr);
  }
};

}  // namespace mirage::ecs

#endif
