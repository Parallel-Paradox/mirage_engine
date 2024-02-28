#ifndef MIRAGE_FRAMEWORK_BASE_AUTO_PTR_SHARED
#define MIRAGE_FRAMEWORK_BASE_AUTO_PTR_SHARED

#include "mirage_framework/base/auto_ptr/destructor.hpp"
#include "mirage_framework/base/auto_ptr/ref_count.hpp"
#include "mirage_framework/define.hpp"

namespace mirage {

template <typename T, AsRefCount R>
class Weak;

template <typename T, AsRefCount R>
class Shared {
 public:
  Shared() = default;
  Shared(const Shared&) = delete;

  template <typename... Args>
  static Shared New(Args&&... args) {
    return Shared(new T(args...));
  }

  explicit Shared(T* raw_ptr)
      : Shared(raw_ptr, new R(), new R(), Destructor::Default<T>()) {
    MIRAGE_DCHECK(raw_ptr != nullptr);
    ref_cnt_->Increase();
  }

  Shared(T* raw_ptr, std::function<void(T*)> destructor)
      : Shared(raw_ptr, new R(), new R(), Destructor::New<T>(destructor)) {
    MIRAGE_DCHECK(raw_ptr != nullptr);
    MIRAGE_DCHECK(destructor != nullptr);
    ref_cnt_->Increase();
  }

  Shared(Shared&& other) noexcept
      : Shared(other.raw_ptr_, other.ref_cnt_, other.weak_ref_cnt_,
               other.destructor_) {
    other.Reset();
  }

  ~Shared() {
    if (ref_cnt_ && ref_cnt_->TryRelease()) {
      destructor_(raw_ptr_);
      if (weak_ref_cnt_->GetCnt() == 0) {
        delete ref_cnt_;
        delete weak_ref_cnt_;
      }
    }
    Reset();
  }

  Shared& operator=(Shared&& other) noexcept {
    if (this != &other) {
      this->~Shared();
      new (this) Shared(std::move(other));
    }
    return *this;
  }

  Shared Clone() const {
    if (ref_cnt_) {
      ref_cnt_->Increase();
      return Shared(raw_ptr_, ref_cnt_, weak_ref_cnt_, destructor_);
    }
    return Shared();
  }

  Shared& operator=(std::nullptr_t) {
    this->~Shared();
    return *this;
  }

  T* operator->() const { return raw_ptr_; }

  T& operator*() const { return *raw_ptr_; }

  T* Get() const { return raw_ptr_; }

  bool IsNull() const { return ref_cnt_ == nullptr || ref_cnt_->GetCnt() == 0; }

  size_t GetRefCnt() const { return ref_cnt_->GetCnt(); }

  size_t GetWeakRefCnt() const { return weak_ref_cnt_->GetCnt(); }

 private:
  friend class Weak<T, R>;

  Shared(T* raw_ptr, RefCount* ref_cnt, RefCount* weak_ref_cnt,
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
using SharedLocal = Shared<T, RefCountLocal>;

template <typename T>
using SharedAsync = Shared<T, RefCountAsync>;

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_AUTO_PTR_SHARED
