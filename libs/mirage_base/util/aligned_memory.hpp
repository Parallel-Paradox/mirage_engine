#ifndef MIRAGE_BASE_UTIL_ALIGNED_MEMORY
#define MIRAGE_BASE_UTIL_ALIGNED_MEMORY

#include <concepts>
#include <cstddef>

namespace mirage::base {

template <std::move_constructible T>
class AlignedMemory {
 public:
  AlignedMemory() = default;
  ~AlignedMemory() = default;

  AlignedMemory(const AlignedMemory&) = delete;
  AlignedMemory(AlignedMemory&&) = delete;

  explicit AlignedMemory(T&& val) { new (GetPtr()) T(std::move(val)); }

  T* GetPtr() { return (T*)mem_; }  // NOLINT: std::byte* to T*

  const T* GetConstPtr() const { return (T*)mem_; }  // NOLINT: std::byte* to T*

  T& GetRef() { return *GetPtr(); }

  const T& GetConstRef() const { return *GetConstPtr(); }

 private:
  alignas(T) std::byte mem_[sizeof(T)]{};
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_UTIL_ALIGNED_MEMORY
