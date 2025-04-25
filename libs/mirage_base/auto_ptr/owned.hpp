#ifndef MIRAGE_BASE_AUTO_PTR_OWNED
#define MIRAGE_BASE_AUTO_PTR_OWNED

#include <cstddef>
#include <utility>

namespace mirage::base {

template <typename T>
class Owned {
 public:
  Owned() = default;
  ~Owned();

  Owned(const Owned&) = delete;
  Owned& operator=(const Owned&) = delete;

  Owned(Owned&& other) noexcept;
  Owned& operator=(Owned&& other) noexcept;

  explicit Owned(T* raw_ptr);
  Owned(std::nullptr_t);  // NOLINT: Signed by nullptr
  Owned& operator=(std::nullptr_t);

  void Reset();

  template <typename... Args>
  static Owned New(Args&&... args);

  template <typename T1>
  Owned<T1> TryConvert() &&;

  template <typename T1>
  Owned<T1> Convert() &&;

  T* operator->() const;
  T& operator*() const;
  T* raw_ptr() const;

  explicit operator bool() const;
  bool operator==(std::nullptr_t) const;
  [[nodiscard]] bool IsNull() const;

 private:
  T* raw_ptr_{nullptr};
};

template <typename T>
Owned<T>::~Owned() {
  Reset();
}

template <typename T>
Owned<T>::Owned(Owned&& other) noexcept : raw_ptr_(other.raw_ptr_) {
  other.raw_ptr_ = nullptr;
}

template <typename T>
Owned<T>& Owned<T>::operator=(Owned&& other) noexcept {
  if (this != &other) {
    Reset();
    new (this) Owned(std::move(other));
  }
  return *this;
}

template <typename T>
Owned<T>::Owned(T* raw_ptr) : raw_ptr_(raw_ptr) {}

template <typename T>
Owned<T>::Owned(std::nullptr_t) : raw_ptr_(nullptr) {}

template <typename T>
Owned<T>& Owned<T>::operator=(std::nullptr_t) {
  Reset();
  return *this;
}

template <typename T>
void Owned<T>::Reset() {
  delete raw_ptr_;
  raw_ptr_ = nullptr;
}

template <typename T>
template <typename... Args>
Owned<T> Owned<T>::New(Args&&... args) {
  return Owned(new T(std::forward<Args>(args)...));
}

template <typename T>
template <typename T1>
Owned<T1> Owned<T>::TryConvert() && {
  T1* raw_ptr = dynamic_cast<T1*>(raw_ptr_);
  if (raw_ptr == nullptr) {
    return nullptr;
  }
  return std::move(*this).template Convert<T1>();
}

template <typename T>
template <typename T1>
Owned<T1> Owned<T>::Convert() && {
  Owned<T1> new_owned = Owned<T1>(static_cast<T1*>(raw_ptr_));
  raw_ptr_ = nullptr;
  return new_owned;
}

template <typename T>
T* Owned<T>::operator->() const {
  return raw_ptr_;
}

template <typename T>
T& Owned<T>::operator*() const {
  return *raw_ptr_;
}

template <typename T>
T* Owned<T>::raw_ptr() const {
  return raw_ptr_;
}

template <typename T>
Owned<T>::operator bool() const {
  return raw_ptr_ != nullptr;
}

template <typename T>
bool Owned<T>::operator==(std::nullptr_t) const {
  return raw_ptr_ == nullptr;
}

template <typename T>
bool Owned<T>::IsNull() const {
  return raw_ptr_ == nullptr;
}

}  // namespace mirage::base

#endif  // MIRAGE_BASE_AUTO_PTR_OWNED
