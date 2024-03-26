#ifndef MIRAGE_FRAMEWORK_BASE_UTIL_ALIGNED_MEMORY
#define MIRAGE_FRAMEWORK_BASE_UTIL_ALIGNED_MEMORY

#include <cstddef>

namespace mirage {

template <typename T>
class AlignedMemory {
 public:
  AlignedMemory() = default;
  ~AlignedMemory() = default;

  AlignedMemory(const AlignedMemory&) = delete;

  T* GetPtr() const { return (T*)mem_; }

 private:
  alignas(T) std::byte mem_[sizeof(T)];
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_UTIL_ALIGNED_MEMORY
