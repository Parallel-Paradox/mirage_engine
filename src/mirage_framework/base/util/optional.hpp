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
      new (buffer_) T(std::move(other.Unwrap()));
    }
  }

  ~Optional() {
    is_valid_ = false;
    ((T*)(buffer_))->~T();
  }

  template <typename... Args>
  static Optional New(Args&&... args) {
    Optional result;
    result.is_valid_ = true;
    new (result.buffer_) T(T(std::forward<Args>(args)...));
    return std::move(result);
  }

  static Optional None() { return Optional(); }

  bool IsValid() const { return is_valid_; }

  T Unwrap() {
    MIRAGE_DCHECK(is_valid_);
    is_valid_ = false;
    return std::move(*(T*)(&buffer_));
  }

 private:
  Optional() = default;

  bool is_valid_{false};
  alignas(T) std::byte buffer_[sizeof(T)];
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_UTIL_OPTIONAL
