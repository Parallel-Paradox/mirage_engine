#ifndef MIRAGE_FRAMEWORK_BASE_CONTAINER_ARRAY
#define MIRAGE_FRAMEWORK_BASE_CONTAINER_ARRAY

#include <concepts>
#include <initializer_list>
#include <iterator>

#include "mirage_framework/base/container/concept.hpp"
#include "mirage_framework/define.hpp"

namespace mirage {

template <BasicValueType T>
class ArrayConstIterator;

template <BasicValueType T>
class ArrayIterator {
 public:
  using iterator_concept = std::contiguous_iterator_tag;
  using iterator_category = std::random_access_iterator_tag;
  using iterator_type = ArrayIterator;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using pointer = value_type*;
  using reference = value_type&;

  ArrayIterator() = default;
  ~ArrayIterator() = default;

  ArrayIterator(const ArrayIterator& other) : ptr_(other.ptr_) {}

  explicit ArrayIterator(pointer ptr) : ptr_(ptr) {}

  reference operator*() const { return *ptr_; }

  pointer operator->() const { return ptr_; }

  reference operator[](difference_type diff) const { return ptr_[diff]; }

  iterator_type& operator++() {
    if (ptr_ != nullptr) {
      ++ptr_;
    }
    return *this;
  }

  iterator_type operator++(int) {
    iterator_type temp(*this);
    ++(*this);
    return temp;
  }

  iterator_type& operator--() {
    --ptr_;
    return *this;
  }

  iterator_type operator--(int) {
    iterator_type temp(*this);
    --(*this);
    return temp;
  }

  iterator_type& operator+=(difference_type diff) {
    ptr_ += diff;
    return *this;
  }

  iterator_type operator+(difference_type diff) const {
    iterator_type temp(*this);
    temp += diff;
    return temp;
  }

  iterator_type& operator-=(difference_type diff) {
    ptr_ -= diff;
    return *this;
  }

  iterator_type operator-(difference_type diff) const {
    iterator_type temp(*this);
    temp -= diff;
    return temp;
  }

  difference_type operator-(const iterator_type& other) const {
    return ptr_ - other.ptr_;
  }

  bool operator==(const iterator_type& other) const {
    return ptr_ == other.ptr_;
  }

  bool operator<(const iterator_type& other) const { return ptr_ < other.ptr_; }

  bool operator>(const iterator_type& other) const { return other < *this; }

  bool operator>=(const iterator_type& other) const { return !(*this < other); }

  bool operator<=(const iterator_type& other) const { return !(other < *this); }

 private:
  friend class ArrayConstIterator<T>;

  pointer ptr_{nullptr};
};

template <BasicValueType T>
ArrayIterator<T> operator+(typename ArrayIterator<T>::difference_type diff,
                           ArrayIterator<T> iter) {
  return iter + diff;
}

template <BasicValueType T>
class ArrayConstIterator {
 public:
  using iterator_concept = std::contiguous_iterator_tag;
  using iterator_category = std::random_access_iterator_tag;
  using iterator_type = ArrayConstIterator;
  using difference_type = ptrdiff_t;
  using value_type = const T;
  using pointer = value_type*;
  using reference = value_type&;

  ArrayConstIterator() = default;
  ~ArrayConstIterator() = default;

  ArrayConstIterator(const ArrayConstIterator& other) : ptr_(other.ptr_) {}

  explicit ArrayConstIterator(const ArrayIterator<T>& iter) : ptr_(iter.ptr_) {}

  explicit ArrayConstIterator(pointer ptr) : ptr_(ptr) {}

  reference operator*() const { return *ptr_; }

  pointer operator->() const { return ptr_; }

  reference operator[](difference_type diff) const { return ptr_[diff]; }

  iterator_type& operator++() {
    if (ptr_ != nullptr) {
      ++ptr_;
    }
    return *this;
  }

  iterator_type operator++(int) {
    iterator_type temp(*this);
    ++(*this);
    return temp;
  }

  iterator_type& operator--() {
    --ptr_;
    return *this;
  }

  iterator_type operator--(int) {
    iterator_type temp(*this);
    --(*this);
    return temp;
  }

  iterator_type& operator+=(difference_type diff) {
    ptr_ += diff;
    return *this;
  }

  iterator_type operator+(difference_type diff) const {
    iterator_type temp(*this);
    temp += diff;
    return temp;
  }

  iterator_type& operator-=(difference_type diff) {
    ptr_ -= diff;
    return *this;
  }

  iterator_type operator-(difference_type diff) const {
    iterator_type temp(*this);
    temp -= diff;
    return temp;
  }

  difference_type operator-(const iterator_type& other) const {
    return ptr_ - other.ptr_;
  }

  bool operator==(const iterator_type& other) const {
    return ptr_ == other.ptr_;
  }

  bool operator<(const iterator_type& other) const { return ptr_ < other.ptr_; }

  bool operator>(const iterator_type& other) const { return other < *this; }

  bool operator>=(const iterator_type& other) const { return !(*this < other); }

  bool operator<=(const iterator_type& other) const { return !(other < *this); }

 private:
  pointer ptr_{nullptr};
};

template <BasicValueType T>
ArrayConstIterator<T> operator+(
    typename ArrayConstIterator<T>::difference_type diff,
    ArrayConstIterator<T> iter) {
  return iter + diff;
}

template <BasicValueType T>
class Array {
 public:
  using Iterator = ArrayIterator<T>;
  using ConstIterator = ArrayConstIterator<T>;

  Array() = default;

  Array(const Array& other) {
    if constexpr (!std::copy_constructible<T>) {
      MIRAGE_DCHECK(false);  // This type is supposed to be copyable.
    } else {
      Reserve(other.size_);
      for (const T& val : other) {
        Push(val);
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
    for (const T& val : list) {
      Push(val);
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

  T* GetRawPtr() const { return data_; }

  size_t GetSize() const { return size_; }

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

  bool IsEmpty() const { return size_ == 0; }

  size_t GetCapacity() const { return capacity_; }

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

  Iterator begin() { return Iterator(data_); }

  Iterator end() { return Iterator(data_ + size_); }

  ConstIterator begin() const { return ConstIterator(data_); }

  ConstIterator end() const { return ConstIterator(data_ + size_); }

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
