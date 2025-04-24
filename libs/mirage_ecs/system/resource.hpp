#ifndef MIRAGE_ECS_SYSTEM_RESOURCE
#define MIRAGE_ECS_SYSTEM_RESOURCE

#include <type_traits>

#include "mirage_base/define.hpp"
#include "mirage_ecs/define.hpp"
#include "mirage_ecs/framework/world.hpp"
#include "mirage_ecs/system/extract.hpp"
#include "mirage_ecs/system/system_context.hpp"
#include "mirage_ecs/util/marker.hpp"

namespace mirage::ecs {

template <typename T>
  requires IsResource<T>
class Res {
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
  static Res<T> From(World& world, [[maybe_unused]] SystemContext& context) {
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
