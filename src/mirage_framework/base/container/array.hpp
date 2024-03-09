#ifndef MIRAGE_FRAMEWORK_BASE_CONTAINER_ARRAY
#define MIRAGE_FRAMEWORK_BASE_CONTAINER_ARRAY

#include <concepts>
#include <initializer_list>

#include "mirage_framework/base/container/concept.hpp"
#include "mirage_framework/define.hpp"

namespace mirage {

template <BasicValueType T>
class Array {
 public:
  Array() = default;

  Array(const Array& other) {
    if constexpr (!std::copy_constructible<T>) {
      MIRAGE_DCHECK(false);  // This type is supposed to be copyable.
    } else {
      size_ = other.size_;
      capacity_ = other.capacity_;
      data_ = new T[capacity_]();
      for (size_t i = 0; i < size_; ++i) {
        (data_ + i)->~T();
        new (data_ + i) T(other.data_[i]);
      }
    }
  }

  Array(Array&& other) noexcept
      : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.size_ = 0;
    other.capacity_ = 0;
    other.data_ = nullptr;
  }

  Array(std::initializer_list<T> list) {
    Reserve(list.size());
    for (const T& item : list) {
      Push(std::move(item));
    }
  }

  ~Array() noexcept { Clear(); }

  void Clear() {
    delete[] data_;
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
  }

  void Push(const T& val) {
    if constexpr (!std::copy_constructible<T>) {
      MIRAGE_DCHECK(false);  // This type is supposed to be copyable.
    } else {
      Emplace(T(val));
    }
  }

  template <typename... Args>
  void Emplace(Args&&... args) {
    EnsureNotFull();
    (data_ + size_)->~T();
    new (data_ + size_) T(std::forward<Args>(args)...);
    ++size_;
  }

  T Pop() {
    MIRAGE_DCHECK(size_ != 0);
    --size_;
    return std::move(data_[size_]);
  }

  T& operator[](size_t index) { return data_[index]; }

  bool operator==(const Array& other) const {
    if (size_ != other.size_) {
      return false;
    }
    if (data_ == other.data_) {
      return true;
    }
    if constexpr (!std::equality_comparable<T>) {
      return false;  // Can't be compared.
    } else {
      for (size_t i = 0; i < size_; ++i) {
        if (data_[i] != other.data_[i]) {
          return false;
        }
      }
      return true;
    }
    return false;
  }

  void Reserve(size_t capacity) {
    if (capacity <= capacity_) {
      return;
    }
    SetCapacity(capacity);
  }

  [[nodiscard]] T* GetRawPtr() const { return data_; }

  [[nodiscard]] size_t GetSize() const { return size_; }

  void SetSize(size_t size) {
    if (size == size_) {
      return;
    } else if (size > size_) {
      Reserve(size);
      size_ = size;
      return;
    }
    while (size_ > size) {
      Pop();
    }
  }

  [[nodiscard]] bool IsEmpty() const { return size_ == 0; }

  [[nodiscard]] size_t GetCapacity() const { return capacity_; }

  void SetCapacity(size_t capacity) {
    if (capacity == capacity_) {
      return;
    }

    T* data = new T[capacity]();
    size_t size = capacity < size_ ? capacity : size_;
    for (size_t i = 0; i < size; ++i) {
      (data + i)->~T();
      new (data + i) T(std::move(data_[i]));
    }
    delete[] data_;

    data_ = data;
    size_ = size;
    capacity_ = capacity;
  }

 private:
  void EnsureNotFull() {
    if (capacity_ == 0) {
      capacity_ = 1;
      data_ = new T[1]();
    } else if (size_ == capacity_) {
      SetCapacity(2 * capacity_);
    }
  }

  T* data_{nullptr};
  size_t size_{0};
  size_t capacity_{0};
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_CONTAINER_ARRAY
