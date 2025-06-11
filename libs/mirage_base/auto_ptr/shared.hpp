#ifndef MIRAGE_BASE_AUTO_PTR_SHARED
#define MIRAGE_BASE_AUTO_PTR_SHARED

#include <cstddef>

#include "mirage_base/auto_ptr/ref_count.hpp"
#include "mirage_base/define/check.hpp"

namespace mirage::base {

template <typename T, IsRefCount R>
class Weak;

template <typename T, IsRefCount R>
class Shared {
 public:
  Shared() = default;
  ~Shared();

  Shared(const Shared&) = delete;
  Shared& operator=(const Shared&) = delete;

  Shared(Shared&& other) noexcept;
  Shared& operator=(Shared&& other) noexcept;

  explicit Shared(T* raw_ptr);
  Shared(std::nullptr_t);  // NOLINT: Convert from nullptr
  Shared& operator=(std::nullptr_t);

  void Reset();

  template <typename... Args>
  static Shared New(Args&&... args);

  Shared Clone() const;

  template <typename T1, IsRefCount R1>
  friend class Shared;

  template <typename T1>
  Shared<T1, R> TryConvert() &&;

  template <typename T1>
  Shared<T1, R> Convert() &&;

  T* operator->() const;
  T& operator*() const;
  T* raw_ptr() const;

  explicit operator bool() const;
  bool operator==(std::nullptr_t) const;
  [[nodiscard]] bool is_null() const;

  [[nodiscard]] size_t ref_cnt() const;
  [[nodiscard]] size_t weak_ref_cnt() const;

 private:
  friend class Weak<T, R>;

  Shared(T* raw_ptr, RefCount* ref_cnt_ptr, RefCount* weak_ref_cnt_ptr);
  void ResetPtr();

  T* raw_ptr_{nullptr};
  RefCount* ref_cnt_ptr_{nullptr};
  RefCount* weak_ref_cnt_ptr_{nullptr};
};

template <typename T>
using SharedLocal = Shared<T, RefCountLocal>;

template <typename T>
using SharedAsync = Shared<T, RefCountAsync>;

template <typename T, IsRefCount R>
Shared<T, R>::~Shared() {
  Reset();
}

template <typename T, IsRefCount R>
Shared<T, R>::Shared(Shared&& other) noexcept
    : raw_ptr_(other.raw_ptr_),
      ref_cnt_ptr_(other.ref_cnt_ptr_),
      weak_ref_cnt_ptr_(other.weak_ref_cnt_ptr_) {
  other.ResetPtr();
}

template <typename T, IsRefCount R>
Shared<T, R>& Shared<T, R>::operator=(Shared&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~Shared();
  new (this) Shared(std::move(other));
  return *this;
}

template <typename T, IsRefCount R>
Shared<T, R>::Shared(T* raw_ptr) : Shared(raw_ptr, new R(1), new R(0)) {
  MIRAGE_DCHECK(raw_ptr != nullptr);
}

template <typename T, IsRefCount R>
Shared<T, R>::Shared(std::nullptr_t) {}

template <typename T, IsRefCount R>
void Shared<T, R>::Reset() {
  if (ref_cnt_ptr_ && ref_cnt_ptr_->TryRelease()) {
    delete raw_ptr_;
    if (weak_ref_cnt_ptr_->cnt() == 0) {
      delete ref_cnt_ptr_;
      delete weak_ref_cnt_ptr_;
    }
  }
  ResetPtr();
}

template <typename T, IsRefCount R>
Shared<T, R>& Shared<T, R>::operator=(std::nullptr_t) {
  Reset();
  return *this;
}

template <typename T, IsRefCount R>
template <typename... Args>
Shared<T, R> Shared<T, R>::New(Args&&... args) {
  return Shared(new T(std::forward<Args>(args)...));
}

template <typename T, IsRefCount R>
Shared<T, R> Shared<T, R>::Clone() const {
  if (ref_cnt_ptr_ && ref_cnt_ptr_->TryIncrease()) {
    return Shared(raw_ptr_, ref_cnt_ptr_, weak_ref_cnt_ptr_);
  }
  return nullptr;
}

template <typename T, IsRefCount R>
template <typename T1>
Shared<T1, R> Shared<T, R>::TryConvert() && {
  T1* raw_ptr = dynamic_cast<T1*>(raw_ptr_);
  if (raw_ptr == nullptr) {
    return nullptr;
  }
  return std::move(*this).template Convert<T1>();
}

template <typename T, IsRefCount R>
template <typename T1>
Shared<T1, R> Shared<T, R>::Convert() && {
  auto rv = Shared<T1, R>(static_cast<T1*>(raw_ptr_), ref_cnt_ptr_,
                          weak_ref_cnt_ptr_);
  ResetPtr();
  return rv;
}

template <typename T, IsRefCount R>
T* Shared<T, R>::operator->() const {
  return raw_ptr_;
}

template <typename T, IsRefCount R>
T& Shared<T, R>::operator*() const {
  return *raw_ptr_;
}

template <typename T, IsRefCount R>
T* Shared<T, R>::raw_ptr() const {
  return raw_ptr_;
}

template <typename T, IsRefCount R>
Shared<T, R>::operator bool() const {
  return raw_ptr_ != nullptr;
}

template <typename T, IsRefCount R>
bool Shared<T, R>::operator==(std::nullptr_t) const {
  return raw_ptr_ == nullptr;
}

template <typename T, IsRefCount R>
bool Shared<T, R>::is_null() const {
  return raw_ptr_ == nullptr;
}

template <typename T, IsRefCount R>
size_t Shared<T, R>::ref_cnt() const {
  return ref_cnt_ptr_->cnt();
}

template <typename T, IsRefCount R>
size_t Shared<T, R>::weak_ref_cnt() const {
  return weak_ref_cnt_ptr_->cnt();
}

template <typename T, IsRefCount R>
Shared<T, R>::Shared(T* raw_ptr, RefCount* ref_cnt_ptr,
                     RefCount* weak_ref_cnt_ptr)
    : raw_ptr_(raw_ptr),
      ref_cnt_ptr_(ref_cnt_ptr),
      weak_ref_cnt_ptr_(weak_ref_cnt_ptr) {}

template <typename T, IsRefCount R>
void Shared<T, R>::ResetPtr() {
  raw_ptr_ = nullptr;
  ref_cnt_ptr_ = nullptr;
  weak_ref_cnt_ptr_ = nullptr;
}

}  // namespace mirage::base

#endif  // MIRAGE_BASE_AUTO_PTR_SHARED
