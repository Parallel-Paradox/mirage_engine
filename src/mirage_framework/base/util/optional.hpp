#ifndef MIRAGE_FRAMEWORK_BASE_UTIL_OPTIONAL
#define MIRAGE_FRAMEWORK_BASE_UTIL_OPTIONAL

#include <concepts>

#include "mirage_framework/define.hpp"

namespace mirage {

template <std::move_constructible T>
class Optional {
 public:
  Optional(const Optional&) = delete;

  Optional(Optional&& other) : is_valid_(other.is_valid_) {
    if (other.is_valid_) {
      new (&val_) T(std::move(other.Unwrap()));
    }
    other.is_valid_ = false;
  }

  ~Optional() {
    is_valid_ = false;
    ((T*)(&val_))->~T();
  }

  template <typename... Args>
  static Optional New(Args&&... args) {
    return Optional(T(std::forward<Args>(args)...));
  }

  static Optional None() { return Optional(); }

  bool IsValid() const { return is_valid_; }

  T Unwrap() {
    MIRAGE_DCHECK(is_valid_);
    return std::move(*(T*)(&val_));
  }

 private:
  using Type = std::aligned_storage<sizeof(T), alignof(T)>;

  Optional() = default;

  Optional(T&& val) : is_valid_(true) { new (&val_) T(std::move(val)); }

  bool is_valid_{false};
  Type val_;
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_UTIL_OPTIONAL
