#ifndef MIRAGE_BASE_UTIL_OPTIONAL
#define MIRAGE_BASE_UTIL_OPTIONAL

#include "mirage_base/define.hpp"
#include "mirage_base/util/aligned_memory.hpp"

namespace mirage::base {

template <std::move_constructible T>
class Optional {
 public:
  Optional(const Optional&) = delete;

  Optional(Optional&& other) noexcept : is_valid_(other.is_valid_) {
    if (other.is_valid_) {
      new (obj_.ptr()) T(std::move(other.Unwrap()));
    }
  }

  explicit Optional(T&& val) : obj_(std::move(val)), is_valid_(true) {}

  Optional& operator=(Optional&& other) noexcept {
    if (this != &other) {
      this->~Optional();
      new (this) Optional(std::move(other));
    }
    return *this;
  }

  ~Optional() {
    if (is_valid_) {
      obj_.ptr()->~T();
    }
    is_valid_ = false;
  }

  template <typename... Args>
  static Optional New(Args&&... args) {
    Optional result;
    result.is_valid_ = true;
    new (result.obj_.ptr()) T(std::forward<Args>(args)...);
    return result;
  }

  static Optional None() { return Optional(); }

  [[nodiscard]] bool is_valid() const { return is_valid_; }

  T Unwrap() {
    MIRAGE_DCHECK(is_valid_);
    is_valid_ = false;
    return std::move(obj_.ref());
  }

 private:
  Optional() = default;

  AlignedMemory<T> obj_;
  bool is_valid_{false};
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_UTIL_OPTIONAL
