#ifndef MIRAGE_BASE_WRAP_PLACE_HOLDER
#define MIRAGE_BASE_WRAP_PLACE_HOLDER

#include <concepts>
#include <cstddef>

namespace mirage::base {

template <std::move_constructible T>
class alignas(T) PlaceHolder {
 public:
  PlaceHolder() = default;
  ~PlaceHolder() = default;

  PlaceHolder(const PlaceHolder&) = delete;
  PlaceHolder(PlaceHolder&&) = delete;

  T* ptr() { return (T*)mem_; }  // NOLINT: std::byte* to T*
  T* operator->() { return ptr(); }

  const T* ptr() const { return (T*)mem_; }  // NOLINT: std::byte* to T*
  const T* operator->() const { return ptr(); }

  T& ref() { return *ptr(); }
  T& operator*() { return ref(); }

  const T& ref() const { return *ptr(); }
  const T& operator*() const { return ref(); }

 private:
  alignas(T) std::byte mem_[sizeof(T)]{};
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_WRAP_PLACE_HOLDER
