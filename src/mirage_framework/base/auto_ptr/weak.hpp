#ifndef MIRAGE_FRAMEWORK_BASE_AUTO_PTR_WEAK
#define MIRAGE_FRAMEWORK_BASE_AUTO_PTR_WEAK

#include "mirage_framework/base/auto_ptr/destructor.hpp"
#include "mirage_framework/base/auto_ptr/shared.hpp"
#include "mirage_framework/define.hpp"

namespace mirage {

template <typename T, AsRefCount R>
class Weak {
 public:
  Weak() = default;
  Weak(const Weak&) = delete;

  explicit Weak(const Shared<T, R>& shared)
      : Weak(shared.raw_ptr_, shared.ref_cnt_, shared.weak_ref_cnt_,
             shared.destructor_) {
    if (IsNull()) {
      return;
    }
    MIRAGE_DCHECK(weak_ref_cnt_ != nullptr);
    weak_ref_cnt_->Increase();
  }

  Weak(Weak&& other) noexcept
      : Weak(other.raw_ptr_, other.ref_cnt_, other.weak_ref_cnt_,
             other.destructor_) {
    other.Reset();
  }

  ~Weak() {
    if (weak_ref_cnt_ && weak_ref_cnt_->TryRelease()) {
      delete ref_cnt_;
      delete weak_ref_cnt_;
    }
    Reset();
  }

  Weak& operator=(std::nullptr_t) {
    this->~Weak();
    return *this;
  }

  Weak& operator=(Weak&& other) {
    if (this != &other) {
      this->~Weak();
      new (this) Weak(std::move(other));
    }
    return *this;
  }

  Weak Clone() const {
    if (weak_ref_cnt_) {
      weak_ref_cnt_->Increase();
      return Weak(raw_ptr_, ref_cnt_, weak_ref_cnt_, destructor_);
    }
    return Weak();
  }

  Shared<T, R> TryUpgrade() const {
    if (ref_cnt_ != nullptr && ref_cnt_->TryIncrease()) {
      return Shared<T, R>(raw_ptr_, ref_cnt_, weak_ref_cnt_, destructor_);
    }
    return Shared<T, R>();
  }

  T* Get() const { return raw_ptr_; }

  bool IsNull() const { return ref_cnt_ == nullptr || ref_cnt_->GetCnt() == 0; }

  size_t GetRefCnt() const { return ref_cnt_->GetCnt(); }

  size_t GetWeakRefCnt() const { return weak_ref_cnt_->GetCnt(); }

 private:
  Weak(T* raw_ptr, RefCount* ref_cnt, RefCount* weak_ref_cnt,
       const Destructor& destructor)
      : raw_ptr_(raw_ptr),
        ref_cnt_(ref_cnt),
        weak_ref_cnt_(weak_ref_cnt),
        destructor_(destructor) {}

  void Reset() {
    raw_ptr_ = nullptr;
    ref_cnt_ = nullptr;
    weak_ref_cnt_ = nullptr;
    destructor_ = Destructor::Default<T>();
  }

  T* raw_ptr_{nullptr};
  RefCount* ref_cnt_{nullptr};
  RefCount* weak_ref_cnt_{nullptr};
  Destructor destructor_{Destructor::Default<T>()};
};

template <typename T>
using WeakLocal = Weak<T, RefCountLocal>;

template <typename T>
using WeakAsync = Weak<T, RefCountAsync>;

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_AUTO_PTR_WEAK
