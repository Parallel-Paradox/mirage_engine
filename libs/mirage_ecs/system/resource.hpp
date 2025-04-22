#ifndef MIRAGE_ECS_SYSTEM_RESOURCE
#define MIRAGE_ECS_SYSTEM_RESOURCE

#include <type_traits>

#include "mirage_ecs/define.hpp"
#include "mirage_ecs/system/extract.hpp"
#include "mirage_ecs/util/marker.hpp"

namespace mirage::ecs {

template <typename T>
  requires IsResource<T>
class Res {
 public:
  Res() = default;
  ~Res() = default;

  Res(T* raw_ptr) : raw_ptr_(raw_ptr) {}

  T& operator*() const { return *raw_ptr_; }
  T* operator->() const { return raw_ptr_; }

  T* raw_ptr() const { return raw_ptr_; }

 private:
  T* raw_ptr_;
};

template <typename T>
struct Extract<Res<T>> {
  static Res<T> From([[maybe_unused]] Context& context) {
    // TODO
    return Res<T>();
  }
};

}  // namespace mirage::ecs

#endif
