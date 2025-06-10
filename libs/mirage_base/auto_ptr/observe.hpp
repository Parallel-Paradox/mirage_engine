#ifndef MIRAGE_BASE_AUTO_PTR_OBSERVE
#define MIRAGE_BASE_AUTO_PTR_OBSERVE

#include "mirage_base/auto_ptr/owned.hpp"
#include "mirage_base/auto_ptr/ref_count.hpp"

namespace mirage::base {

template <typename T>
class ObserverLocal {
 public:
  ObserverLocal() = default;
  ~ObserverLocal();

  ObserverLocal(const ObserverLocal&) = delete;
  ObserverLocal& operator=(const ObserverLocal&) = delete;

  ObserverLocal(ObserverLocal&& other) noexcept;
  ObserverLocal& operator=(ObserverLocal&& other) noexcept;

  ObserverLocal(std::nullptr_t);  // NOLINT: Convert from nullptr
  ObserverLocal& operator=(std::nullptr_t);

  void Reset();
  ObserverLocal Clone() const;

  template <typename T1>
  ObserverLocal<T1> TryConvert() &&;

  template <typename T1>
  ObserverLocal<T1> Convert() &&;

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
class ObserveCenterLocal {
 public:
  ObserveCenterLocal() = default;
  ~ObserveCenterLocal();

  ObserveCenterLocal(const ObserveCenterLocal&) = delete;
  ObserveCenterLocal& operator=(const ObserveCenterLocal&) = delete;

  ObserveCenterLocal(ObserveCenterLocal&&) noexcept;
  ObserveCenterLocal& operator=(ObserveCenterLocal&&) noexcept;

  explicit ObserveCenterLocal(T* raw_ptr);
  ObserveCenterLocal(std::nullptr_t);  // NOLINT: Convert from nullptr
  ObserveCenterLocal& operator=(std::nullptr_t);

  void Reset();

  template <typename... Args>
  static ObserveCenterLocal New(Args&&... args);

  template <typename T1>
  ObserveCenterLocal<T1> TryConvert() &&;

  template <typename T1>
  ObserveCenterLocal<T1> Convert() &&;

  ObserverLocal<T> NewObserver() const;

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
class ObserveCenterAsync {
  // TODO
  // TODO: Guard with rwlock
};

template <typename T>
class ObserverAsync {
  // TODO
};

template <typename T>
ObserverLocal<T>::~ObserverLocal() {
  Reset();
}

template <typename T>
ObserverLocal<T>::ObserverLocal(ObserverLocal&& other) noexcept
    : raw_ptr_(other.raw_ptr_),
      is_null_(other.is_null_),
      observer_cnt_(other.observer_cnt_) {
  other.raw_ptr_ = nullptr;
  other.is_null_ = nullptr;
  other.observer_cnt_ = nullptr;
}

template <typename T>
ObserverLocal<T>& ObserverLocal<T>::operator=(ObserverLocal&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~ObserverLocal();
  new (this) ObserverLocal(std::move(other));
  return *this;
}

template <typename T>
void ObserverLocal<T>::Reset() {
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
