#ifndef MIRAGE_BASE_AUTO_PTR_OBSERVED
#define MIRAGE_BASE_AUTO_PTR_OBSERVED

#include <atomic>

#include "mirage_base/auto_ptr/owned.hpp"
#include "mirage_base/auto_ptr/ref_count.hpp"
#include "mirage_base/define/check.hpp"
#include "mirage_base/sync/rw_lock.hpp"

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
  friend class ObservedLocal;

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
  ObservedLocal(Owned<T> self, bool* is_null, RefCount* observer_cnt);
  void ResetPtr();

  T* raw_ptr_{nullptr};
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
  friend class LocalObserver;

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
 public:
  class ReadGuard;
  class WriteGuard;

  ObservedAsync() = default;
  ~ObservedAsync();

  ObservedAsync(const ObservedAsync&) = delete;
  ObservedAsync& operator=(const ObservedAsync&) = delete;

  ObservedAsync(ObservedAsync&&) noexcept;
  ObservedAsync& operator=(ObservedAsync&&) noexcept;

  explicit ObservedAsync(T* raw_ptr);
  ObservedAsync(std::nullptr_t);  // NOLINT: Convert from nullptr
  ObservedAsync& operator=(std::nullptr_t);

  void Reset();

  template <typename... Args>
  static ObservedAsync New(Args&&... args);

  template <typename T1>
  friend class ObservedAsync;

  template <typename T1>
  ObservedAsync<T1> TryConvert() &&;

  template <typename T1>
  ObservedAsync<T1> Convert() &&;

  AsyncObserver<T> NewObserver() const;

  ReadGuard Read() const;
  WriteGuard Write() const;

  T* raw_ptr() const;

  explicit operator bool() const;
  bool operator==(std::nullptr_t) const;
  [[nodiscard]] bool is_null() const;

  [[nodiscard]] size_t observer_cnt() const;

 private:
  Owned<T> self_{nullptr};
  RWLock* rw_lock_{nullptr};
  std::atomic_flag* is_null_{nullptr};
  RefCount* observer_cnt_{nullptr};
};

template <typename T>
class AsyncObserver {
 public:
  AsyncObserver() = default;
  ~AsyncObserver();

  AsyncObserver(const AsyncObserver&) = delete;
  AsyncObserver& operator=(const AsyncObserver&) = delete;

  AsyncObserver(AsyncObserver&& other) noexcept;
  AsyncObserver& operator=(AsyncObserver&& other) noexcept;

  AsyncObserver(std::nullptr_t);  // NOLINT: Convert from nullptr
  AsyncObserver& operator=(std::nullptr_t);

  void Reset();
  AsyncObserver Clone() const;

  template <typename T1>
  friend class AsyncObserver;

  template <typename T1>
  AsyncObserver<T1> TryConvert() &&;

  template <typename T1>
  AsyncObserver<T1> Convert() &&;

  ReadGuard Read() const;
  WriteGuard Write() const;

  T* raw_ptr() const;

  explicit operator bool() const;
  bool operator==(std::nullptr_t) const;
  [[nodiscard]] bool is_null() const;

  [[nodiscard]] size_t observer_cnt() const;

 private:
  T* raw_ptr_{nullptr};
  RWLock* rw_lock_{nullptr};
  std::atomic_flag* is_null_{nullptr};
  RefCount* observer_cnt_{nullptr};
};

template <typename T>
class ObservedAsync<T>::ReadGuard {
 public:
  ReadGuard() = delete;
  ~ReadGuard();

  ReadGuard(const ReadGuard&) = delete;
  ReadGuard& operator=(const ReadGuard&) = delete;

  ReadGuard(ReadGuard&& other) noexcept;
  ReadGuard& operator=(ReadGuard&& other) noexcept;

  const T* operator->() const;
  const T& operator*() const;

 private:
  friend class ObservedAsync<T>;
  friend class AsyncObserver<T>;

  ReadGuard(const T* raw_ptr, const RWLock& rw_lock);

  const T* raw_ptr_;
  base::ReadGuard read_guard_;
};

template <typename T>
class ObservedAsync<T>::WriteGuard {
 public:
  WriteGuard() = delete;
  ~WriteGuard();

  WriteGuard(const WriteGuard&) = delete;
  WriteGuard& operator=(const WriteGuard&) = delete;

  WriteGuard(WriteGuard&& other) noexcept;
  WriteGuard& operator=(WriteGuard&& other) noexcept;

  T* operator->() const;
  T& operator*() const;

 private:
  friend class ObservedAsync<T>;
  friend class AsyncObserver<T>;

  WriteGuard(T* raw_ptr, const RWLock& rw_lock);

  T* raw_ptr_;
  base::WriteGuard write_guard_;
};

template <typename T>
ObservedLocal<T>::~ObservedLocal() {
  Reset();
}

template <typename T>
ObservedLocal<T>::ObservedLocal(ObservedLocal&& other) noexcept
    : raw_ptr_(other.raw_ptr_),
      is_null_(other.is_null_),
      observer_cnt_(other.observer_cnt_) {
  other.ResetPtr();
}

template <typename T>
ObservedLocal<T>& ObservedLocal<T>::operator=(ObservedLocal&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~ObservedLocal();
  new (this) ObservedLocal(std::move(other));
  return *this;
}

template <typename T>
ObservedLocal<T>::ObservedLocal(T* raw_ptr)
    : raw_ptr_(raw_ptr),
      is_null_(new bool(false)),
      observer_cnt_(new RefCountLocal(0)) {
  MIRAGE_DCHECK(raw_ptr != nullptr);
}

template <typename T>
ObservedLocal<T>::ObservedLocal(std::nullptr_t)
    : raw_ptr_(nullptr), is_null_(nullptr), observer_cnt_(nullptr) {}

template <typename T>
ObservedLocal<T>& ObservedLocal<T>::operator=(std::nullptr_t) {
  Reset();
  return *this;
}

template <typename T>
void ObservedLocal<T>::Reset() {
  if (is_null()) {
    return;
  }
  delete raw_ptr_;
  if (observer_cnt_->cnt() == 0) {
    delete is_null_;
    delete observer_cnt_;
  }
  ResetPtr();
}

template <typename T>
template <typename... Args>
ObservedLocal<T> ObservedLocal<T>::New(Args&&... args) {
  return ObservedLocal<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
template <typename T1>
ObservedLocal<T1> ObservedLocal<T>::TryConvert() && {
  T1* raw_ptr = dynamic_cast<T1*>(raw_ptr_);
  if (raw_ptr == nullptr) {
    return nullptr;
  }
  return std::move(*this).template Convert<T1>();
}

template <typename T>
template <typename T1>
ObservedLocal<T1> ObservedLocal<T>::Convert() && {
  auto rv = ObservedLocal<T1>(raw_ptr_, is_null_, observer_cnt_);
  ResetPtr();
  return rv;
}

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

#endif  // MIRAGE_BASE_AUTO_PTR_OBSERVED
