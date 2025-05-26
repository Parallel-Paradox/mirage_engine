#ifndef MIRAGE_BASE_WRAP_BOX
#define MIRAGE_BASE_WRAP_BOX

#include <concepts>

#include "mirage_base/define/check.hpp"
#include "mirage_base/define/export.hpp"
#include "mirage_base/util/type_id.hpp"

namespace mirage::base {

class MIRAGE_BASE Box {
 public:
  Box() = default;
  ~Box();

  Box(const Box&) = delete;
  Box& operator=(const Box&) = delete;

  Box(Box&& other) noexcept;
  Box& operator=(Box&& other) noexcept;

  template <std::move_constructible T>
  static Box New(T val);

  template <std::move_constructible T>
  Box& operator=(T val);

  template <std::move_constructible T>
  T Unwrap();

  template <typename T>
  T* TryCast();

  template <typename T>
  const T* TryCast() const;

  void Reset();

  [[nodiscard]] bool is_valid() const;
  [[nodiscard]] TypeId type_id() const;

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
                            const TypeMeta* type_meta);
  template <typename T>
    requires /* check soo */ (Box::AllowSmallObjectOptimize<T>())
  static void* SmallHandler(Action action, Box* target, Box* dest,
                            const TypeMeta* type_meta);

  void* Call(Action action, Box* dest = nullptr,
             const TypeMeta* type_meta = nullptr) const;

  union {
    void* ptr{nullptr};
    std::byte buffer[3 * sizeof(void*)];
  } obj_;
  HandleFuncPtr handle_func_{nullptr};
};

template <std::move_constructible T>
Box Box::New(T val) {
  Box box;
  if constexpr (AllowSmallObjectOptimize<T>()) {
    box.handle_func_ = SmallHandler<T>;
    new (&box.obj_.buffer) T(std::move(val));
  } else {
    box.handle_func_ = LargeHandler<T>;
    box.obj_.ptr = new T(std::move(val));
  }
  return box;
}

template <std::move_constructible T>
Box& Box::operator=(T val) {
  this->~Box();
  new (this) Box(New<T>(std::move(val)));
  return *this;
}

template <std::move_constructible T>
T Box::Unwrap() {
  MIRAGE_DCHECK(is_valid());

  T* ptr = TryCast<T>();
  MIRAGE_DCHECK(ptr != nullptr);

  return std::move(*ptr);
}

template <typename T>
T* Box::TryCast() {
  return static_cast<T*>(Call(kGet, nullptr, &TypeMeta::Of<T>()));
}

template <typename T>
const T* Box::TryCast() const {
  return static_cast<const T*>(Call(kGet, nullptr, &TypeMeta::Of<T>()));
}

template <typename T>
void* Box::LargeHandler(const Action action, Box* target, Box* dest,
                        const TypeMeta* type_meta) {
  switch (action) {
    case kMove:
      dest->obj_.ptr = target->obj_.ptr;
      target->obj_.ptr = nullptr;
      break;
    case kDestruct:
      delete static_cast<T*>(target->obj_.ptr);
      break;
    case kGet:
      MIRAGE_DCHECK(type_meta != nullptr);
      if (target->type_id() == TypeId(*type_meta)) {
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
void* Box::SmallHandler(const Action action, Box* target, Box* dest,
                        const TypeMeta* type_meta) {
  switch (action) {
    case kMove:
      new (&dest->obj_.buffer)
          T(std::move(*reinterpret_cast<T*>(&target->obj_.buffer)));
      break;
    case kDestruct:
      reinterpret_cast<T*>(&target->obj_.buffer)->~T();
      break;
    case kGet:
      MIRAGE_DCHECK(type_meta != nullptr);
      if (target->type_id() == TypeId(*type_meta)) {
        return &target->obj_.buffer;
      }
      break;
    case kTypeMeta:
      return const_cast<TypeMeta*>(&TypeMeta::Of<T>());
  }
  return nullptr;
}

}  // namespace mirage::base

#endif  // MIRAGE_BASE_WRAP_BOX
