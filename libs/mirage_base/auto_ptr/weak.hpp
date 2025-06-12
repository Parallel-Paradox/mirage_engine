#ifndef MIRAGE_BASE_AUTO_PTR_WEAK
#define MIRAGE_BASE_AUTO_PTR_WEAK

#include "mirage_base/auto_ptr/ref_count.hpp"
#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/define/check.hpp"

namespace mirage::base {

template <typename T, IsRefCount R>
class Weak {
 public:
  Weak() = default;
  ~Weak();

  Weak(const Weak&) = delete;
  Weak& operator=(const Weak&) = delete;

  Weak(Weak&& other) noexcept;
  Weak& operator=(Weak&& other) noexcept;

  explicit Weak(const Shared<T, R>& other);

  Weak(std::nullptr_t);  // NOLINT: Convert from nullptr
  Weak& operator=(std::nullptr_t);

  void Reset();

  Weak Clone() const;

  template <typename T1, IsRefCount R1>
  friend class Weak;

  template <typename T1>
  Weak<T1, R> TryConvert();

  template <typename T1>
  Weak<T1, R> Convert() &&;

  Shared<T, R> TryUpgrade() const;

  T* raw_ptr() const;

  explicit operator bool() const;
  bool operator==(std::nullptr_t) const;
  [[nodiscard]] bool is_null() const;

  [[nodiscard]] size_t ref_cnt() const;
  [[nodiscard]] size_t weak_ref_cnt() const;

 private:
  Weak(T* raw_ptr, RefCount* ref_cnt_ptr, RefCount* weak_ref_cnt_ptr);
  void ResetPtr();

  T* raw_ptr_{nullptr};
  RefCount* ref_cnt_ptr_{nullptr};
  RefCount* weak_ref_cnt_ptr_{nullptr};
};

template <typename T>
using WeakLocal = Weak<T, RefCountLocal>;

template <typename T>
using WeakAsync = Weak<T, RefCountAsync>;

template <typename T, IsRefCount R>
Weak<T, R>::~Weak() {
  Reset();
}

template <typename T, IsRefCount R>
Weak<T, R>::Weak(Weak&& other) noexcept
    : raw_ptr_(other.raw_ptr_),
      ref_cnt_ptr_(other.ref_cnt_ptr_),
      weak_ref_cnt_ptr_(other.weak_ref_cnt_ptr_) {
  other.ResetPtr();
}

template <typename T, IsRefCount R>
Weak<T, R>& Weak<T, R>::operator=(Weak&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~Weak();
  new (this) Weak(std::move(other));
  return *this;
}

template <typename T, IsRefCount R>
Weak<T, R>::Weak(const Shared<T, R>& other)
    : raw_ptr_(other.raw_ptr_),
      ref_cnt_ptr_(other.ref_cnt_ptr_),
      weak_ref_cnt_ptr_(other.weak_ref_cnt_ptr_) {
  if (is_null()) {
    return;
  }
  MIRAGE_DCHECK(weak_ref_cnt_ptr_ != nullptr);
  weak_ref_cnt_ptr_->Increase();
}

template <typename T, IsRefCount R>
Weak<T, R>::Weak(std::nullptr_t) {}

template <typename T, IsRefCount R>
Weak<T, R>& Weak<T, R>::operator=(std::nullptr_t) {
  Reset();
  return *this;
}

template <typename T, IsRefCount R>
void Weak<T, R>::Reset() {
  if (weak_ref_cnt_ptr_ && weak_ref_cnt_ptr_->TryRelease() &&
      ref_cnt_ptr_->cnt() == 0) {
    delete ref_cnt_ptr_;
    delete weak_ref_cnt_ptr_;
  }
  ResetPtr();
}

template <typename T, IsRefCount R>
Weak<T, R> Weak<T, R>::Clone() const {
  if (weak_ref_cnt_ptr_) {
    weak_ref_cnt_ptr_->Increase();
    return Weak(raw_ptr_, ref_cnt_ptr_, weak_ref_cnt_ptr_);
  }
  return nullptr;
}

template <typename T, IsRefCount R>
template <typename T1>
Weak<T1, R> Weak<T, R>::TryConvert() {
  T1* raw_ptr = dynamic_cast<T1*>(raw_ptr_);
  if (raw_ptr == nullptr) {
    return nullptr;
  }
  return std::move(*this).template Convert<T1>();
}

template <typename T, IsRefCount R>
template <typename T1>
Weak<T1, R> Weak<T, R>::Convert() && {
  auto rv =
      Weak<T1, R>(static_cast<T1*>(raw_ptr_), ref_cnt_ptr_, weak_ref_cnt_ptr_);
  ResetPtr();
  return rv;
}

template <typename T, IsRefCount R>
Shared<T, R> Weak<T, R>::TryUpgrade() const {
  if (ref_cnt_ptr_ && ref_cnt_ptr_->TryIncrease()) {
    return Shared<T, R>(raw_ptr_, ref_cnt_ptr_, weak_ref_cnt_ptr_);
  }
  return nullptr;
}

template <typename T, IsRefCount R>
T* Weak<T, R>::raw_ptr() const {
  return raw_ptr_;
}

template <typename T, IsRefCount R>
Weak<T, R>::operator bool() const {
  return !is_null();
}

template <typename T, IsRefCount R>
bool Weak<T, R>::operator==(std::nullptr_t) const {
  return is_null();
}

template <typename T, IsRefCount R>
bool Weak<T, R>::is_null() const {
  return ref_cnt_ptr_ == nullptr || ref_cnt_ptr_->cnt() == 0;
}

template <typename T, IsRefCount R>
size_t Weak<T, R>::ref_cnt() const {
  return ref_cnt_ptr_->cnt();
}

template <typename T, IsRefCount R>
size_t Weak<T, R>::weak_ref_cnt() const {
  return weak_ref_cnt_ptr_->cnt();
}

template <typename T, IsRefCount R>
Weak<T, R>::Weak(T* raw_ptr, RefCount* ref_cnt_ptr, RefCount* weak_ref_cnt_ptr)
    : raw_ptr_(raw_ptr),
      ref_cnt_ptr_(ref_cnt_ptr),
      weak_ref_cnt_ptr_(weak_ref_cnt_ptr) {}

template <typename T, IsRefCount R>
void Weak<T, R>::ResetPtr() {
  raw_ptr_ = nullptr;
  ref_cnt_ptr_ = nullptr;
  weak_ref_cnt_ptr_ = nullptr;
}

}  // namespace mirage::base

#endif  // MIRAGE_BASE_AUTO_PTR_WEAK
