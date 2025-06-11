#ifndef MIRAGE_BASE_AUTO_PTR_OBSERVE
#define MIRAGE_BASE_AUTO_PTR_OBSERVE

#include "mirage_base/auto_ptr/owned.hpp"
#include "mirage_base/auto_ptr/ref_count.hpp"

namespace mirage::base {

template <typename T>
class LocalObserver;

template <typename T>
class ObservedLocal {
 public:
  ObservedLocal() = default;
  ~ObservedLocal();

  ObservedLocal(const ObservedLocal&) = delete;
  ObservedLocal& operator=(const ObservedLocal&) = delete;

  ObservedLocal(ObservedLocal&&) noexcept;
  ObservedLocal& operator=(ObservedLocal&&) noexcept;

  explicit ObservedLocal(T* raw_ptr);
  ObservedLocal(std::nullptr_t);  // NOLINT: Convert from nullptr
  ObservedLocal& operator=(std::nullptr_t);

  void Reset();

  template <typename... Args>
  static ObservedLocal New(Args&&... args);

  template <typename T1>
  ObservedLocal<T1> TryConvert() &&;

  template <typename T1>
  ObservedLocal<T1> Convert() &&;

  LocalObserver<T> NewObserver() const;

  T* operator->() const;
  T& operator*() const;
  T* raw_ptr() const;

  explicit operator bool() const;
  bool operator==(std::nullptr_t) const;
  [[nodiscard]] bool is_null() const;

  [[nodiscard]] size_t observer_cnt() const;

 private:
  Owned<T> self_{nullptr};
  bool* is_null_{nullptr};
  RefCount* observer_cnt_{nullptr};
};

template <typename T>
class LocalObserver {
 public:
  LocalObserver() = default;
  ~LocalObserver();

  LocalObserver(const LocalObserver&) = delete;
  LocalObserver& operator=(const LocalObserver&) = delete;

  LocalObserver(LocalObserver&& other) noexcept;
  LocalObserver& operator=(LocalObserver&& other) noexcept;

  LocalObserver(std::nullptr_t);  // NOLINT: Convert from nullptr
  LocalObserver& operator=(std::nullptr_t);

  void Reset();
  LocalObserver Clone() const;

  template <typename T1>
  LocalObserver<T1> TryConvert() &&;

  template <typename T1>
  LocalObserver<T1> Convert() &&;

  T* operator->() const;
  T& operator*() const;
  T* raw_ptr() const;

  explicit operator bool() const;
  bool operator==(std::nullptr_t) const;
  [[nodiscard]] bool is_null() const;

  [[nodiscard]] size_t observer_cnt() const;

 private:
  T* raw_ptr_{nullptr};
  bool* is_null_{nullptr};
  RefCount* observer_cnt_{nullptr};
};

template <typename T>
class AsyncObserver;

template <typename T>
class ObservedAsync {
  // TODO
  // TODO: Guard with rwlock
};

template <typename T>
class AsyncObserver {
 public:
  // TODO
};

template <typename T>
LocalObserver<T>::~LocalObserver() {
  Reset();
}

template <typename T>
LocalObserver<T>::LocalObserver(LocalObserver&& other) noexcept
    : raw_ptr_(other.raw_ptr_),
      is_null_(other.is_null_),
      observer_cnt_(other.observer_cnt_) {
  other.raw_ptr_ = nullptr;
  other.is_null_ = nullptr;
  other.observer_cnt_ = nullptr;
}

template <typename T>
LocalObserver<T>& LocalObserver<T>::operator=(LocalObserver&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~LocalObserver();
  new (this) LocalObserver(std::move(other));
  return *this;
}

template <typename T>
void LocalObserver<T>::Reset() {
  if (!observer_cnt_) {
    return;
  }
  if (observer_cnt_->TryRelease()) {
    delete is_null_;
    delete observer_cnt_;
  }
  raw_ptr_ = nullptr;
  is_null_ = nullptr;
  observer_cnt_ = nullptr;
}

}  // namespace mirage::base

#endif  // MIRAGE_BASE_AUTO_PTR_OBSERVE
