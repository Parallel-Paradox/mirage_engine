#ifndef MIRAGE_BASE_WRAP_BOX
#define MIRAGE_BASE_WRAP_BOX

#include <concepts>
#include <type_traits>

#include "mirage_base/define/check.hpp"
#include "mirage_base/util/type_id.hpp"

namespace mirage::base {

template <typename Base>
class Box {
 public:
  Box() = default;
  ~Box() { Reset(); }

  Box(const Box&) = delete;
  Box& operator=(const Box&) = delete;

  Box(Box&& other) noexcept {
    if (other.handle_func_ != nullptr) {
      other.Call(kMove, this);
    }
  }

  Box& operator=(Box&& other) noexcept {
    if (this == &other) {
      return *this;
    }
    this->~Box();
    new (this) Box(std::move(other));
    return *this;
  }

  template <typename T>
    requires std::same_as<Base, void> || (!std::is_reference_v<T>) ||
             (std::move_constructible<T> && std::derived_from<T, Base>)
  Box(T val) {
    if constexpr (AllowSmallObjectOptimize<T>()) {
      handle_func_ = SmallHandler<T>;
      new (&obj_.buffer) T(std::move(val));
    } else {
      handle_func_ = LargeHandler<T>;
      obj_.ptr = new T(std::move(val));
    }
  }

  template <typename T>
    requires std::same_as<Base, void> || (!std::is_reference_v<T>) ||
             (std::move_constructible<T> && std::derived_from<T, Base>)
  Box& operator=(T val) {
    this->~Box();
    new (this) Box(std::move(val));
    return *this;
  }

  template <typename T>
    requires std::same_as<Base, void> ||
             (std::move_constructible<T> && std::derived_from<T, Base>)
  T Unwrap() {
    MIRAGE_DCHECK(is_valid());

    T* ptr = TryCast<T>();
    MIRAGE_DCHECK(ptr != nullptr);

    T rv = std::move(*ptr);
    Reset();
    return rv;
  }

  template <typename T>
    requires std::same_as<Base, void> || std::derived_from<T, Base>
  T* TryCast() {
    return static_cast<T*>(Call(kGet, nullptr, &TypeMeta::Of<T>()));
  }

  template <typename T>
    requires std::same_as<Base, void> || std::derived_from<T, Base>
  const T* TryCast() const {
    return static_cast<const T*>(Call(kGet, nullptr, &TypeMeta::Of<T>()));
  }

  void Reset() {
    if (handle_func_ == nullptr) return;
    Call(kDestruct);
    obj_.ptr = nullptr;
    handle_func_ = nullptr;
  }

  [[nodiscard]] bool is_valid() const { return handle_func_ != nullptr; }

  [[nodiscard]] TypeId type_id() const {
    MIRAGE_DCHECK(is_valid());
    return TypeId(*static_cast<const TypeMeta*>(Call(kTypeMeta)));
  }

  Base* raw_ptr() { return static_cast<Base*>(Call(kGet)); }
  const Base* raw_ptr() const { return static_cast<const Base*>(Call(kGet)); }

  template <typename T>
  consteval static bool AllowSmallObjectOptimize() {
    return std::move_constructible<T> &&
           (alignof(decltype(obj_)) % alignof(T) == 0) &&
           (sizeof(T) <= sizeof(decltype(obj_)));
  }

 private:
  enum Action {
    kMove,
    kDestruct,
    kGet,
    kTypeMeta,
  };

  using HandleFuncPtr = void* (*)(Action action, Box* target, Box* dest,
                                  const TypeMeta* type_meta);

  template <typename T>
  static void* LargeHandler(Action action, Box* target, Box* dest,
                            const TypeMeta* type_meta) {
    switch (action) {
      case kMove:
        dest->obj_.ptr = target->obj_.ptr;
        dest->handle_func_ = target->handle_func_;
        target->obj_.ptr = nullptr;
        target->handle_func_ = nullptr;
        break;
      case kDestruct:
        delete static_cast<T*>(target->obj_.ptr);
        break;
      case kGet:
        if (type_meta == nullptr || target->type_id() == TypeId(*type_meta)) {
          return target->obj_.ptr;
        }
        break;
      case kTypeMeta:
        return const_cast<TypeMeta*>(&TypeMeta::Of<T>());
    }
    return nullptr;
  }

  template <typename T>
    requires /* check soo */ (Box::AllowSmallObjectOptimize<T>())
  static void* SmallHandler(Action action, Box* target, Box* dest,
                            const TypeMeta* type_meta) {
    T* ptr = reinterpret_cast<T*>(&target->obj_.buffer);
    switch (action) {
      case kMove:
        new (&dest->obj_.buffer) T(std::move(*ptr));
        dest->handle_func_ = target->handle_func_;
        ptr->~T();
        target->handle_func_ = nullptr;
        break;
      case kDestruct:
        ptr->~T();
        break;
      case kGet:
        if (type_meta == nullptr || target->type_id() == TypeId(*type_meta)) {
          return ptr;
        }
        break;
      case kTypeMeta:
        return const_cast<TypeMeta*>(&TypeMeta::Of<T>());
    }
    return nullptr;
  }

  void* Call(Action action, Box* dest = nullptr,
             const TypeMeta* type_meta = nullptr) const {
    return handle_func_(action, const_cast<Box*>(this), dest, type_meta);
  }

  union {
    void* ptr{nullptr};
    std::byte buffer[3 * sizeof(void*)];
  } obj_;
  HandleFuncPtr handle_func_{nullptr};
};

using BoxAny = Box<void>;

}  // namespace mirage::base

#endif  // MIRAGE_BASE_WRAP_BOX
