#ifndef MIRAGE_BASE_WRAP_BOX
#define MIRAGE_BASE_WRAP_BOX

#include <concepts>

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
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_WRAP_BOX
