#ifndef MIRAGE_BASE_WRAP_BOX
#define MIRAGE_BASE_WRAP_BOX

#include <concepts>

#include "mirage_base/util/type_id.hpp"

namespace mirage::base {

class Box {
 public:
  Box() = default;
  ~Box();

  Box(const Box&) = delete;
  Box& operator=(const Box&) = delete;

  Box(Box&& other) noexcept;
  Box& operator=(Box&& other) noexcept;

  template <std::move_constructible T>
  explicit Box(T val);

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
  [[nodiscard]] bool is_soo() const;  // small object optimization

 private:
  union {
    void* ptr{nullptr};
    std::byte buffer[3 * sizeof(void*)];
  } obj_;

  template <typename T>
  consteval static bool AllowSmallObjectOptimize() {
    return std::move_constructible<T> &&
           (alignof(decltype(obj_)) % alignof(T) == 0) &&
           (sizeof(T) <= sizeof(decltype(obj_)));
  }

  enum Action {
    kMove,
    kDestruct,
    kGet,
    kTypeId,
  };

  using HandleFuncPtr = void* (*)(Action action, Box* target, Box* dest,
                                  const TypeMeta* type_meta);
  template <typename T>
  static void* HandleFunc(Action action, Box* target, Box* dest,
                          const TypeMeta* type_meta);
  template <typename T>
    requires /* check soo */ (AllowSmallObjectOptimize<T>())
  static void* SooHandleFunc(Action action, Box* target, Box* dest,
                             const TypeMeta* type_meta);

  HandleFuncPtr handle_func_{nullptr};
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_WRAP_BOX
