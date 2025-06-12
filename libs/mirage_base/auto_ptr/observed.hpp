#ifndef MIRAGE_BASE_AUTO_PTR_OBSERVED
#define MIRAGE_BASE_AUTO_PTR_OBSERVED

#include <atomic>

#include "mirage_base/auto_ptr/owned.hpp"
#include "mirage_base/auto_ptr/ref_count.hpp"
#include "mirage_base/define/check.hpp"
#include "mirage_base/sync/rw_lock.hpp"
#include "mirage_base/wrap/optional.hpp"

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
  ObservedLocal<T1> TryConvert();

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
  ObservedLocal(T* raw_ptr, bool* is_null, RefCount* observer_cnt);
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
  LocalObserver<T1> TryConvert();

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
  friend class ObservedLocal<T>;

  LocalObserver(T* raw_ptr, bool* is_null, RefCount* observer_cnt);
  void ResetPtr();

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
  ObservedAsync<T1> TryConvert();

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
  ObservedAsync(T* raw_ptr, RWLock* rw_lock, bool* is_null,
                RefCount* observer_cnt);
  void ResetPtr();

  T* raw_ptr_{nullptr};
  RWLock* rw_lock_{nullptr};
  bool* is_null_{nullptr};
  RefCount* observer_cnt_{nullptr};
};

template <typename T>
class AsyncObserver {
 public:
  using ReadGuard = typename ObservedAsync<T>::ReadGuard;
  using WriteGuard = typename ObservedAsync<T>::WriteGuard;

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
  AsyncObserver<T1> TryConvert();

  template <typename T1>
  AsyncObserver<T1> Convert() &&;

  Optional<ReadGuard> Read() const;
  Optional<WriteGuard> Write() const;

  T* raw_ptr() const;

  explicit operator bool() const;
  bool operator==(std::nullptr_t) const;
  [[nodiscard]] bool is_null() const;

  [[nodiscard]] size_t observer_cnt() const;

 private:
  friend class ObservedAsync<T>;
  AsyncObserver(T* raw_ptr, RWLock* rw_lock, bool* is_null,
                RefCount* observer_cnt);
  void ResetPtr();

  T* raw_ptr_{nullptr};
  RWLock* rw_lock_{nullptr};
  bool* is_null_{nullptr};
  RefCount* observer_cnt_{nullptr};
};

template <typename T>
class ObservedAsync<T>::ReadGuard {
 public:
  ReadGuard() = delete;
  ~ReadGuard() = default;

  ReadGuard(const ReadGuard&) = delete;
  ReadGuard& operator=(const ReadGuard&) = delete;

  ReadGuard(ReadGuard&& other) noexcept;
  ReadGuard& operator=(ReadGuard&& other) noexcept;

  const T* operator->() const;
  const T& operator*() const;

 private:
  friend class ObservedAsync<T>;
  friend class AsyncObserver<T>;

  ReadGuard(const T* raw_ptr, base::ReadGuard&& rw_lock);

  const T* raw_ptr_;
  base::ReadGuard read_guard_;
};

template <typename T>
class ObservedAsync<T>::WriteGuard {
 public:
  WriteGuard() = delete;
  ~WriteGuard() = default;

  WriteGuard(const WriteGuard&) = delete;
  WriteGuard& operator=(const WriteGuard&) = delete;

  WriteGuard(WriteGuard&& other) noexcept;
  WriteGuard& operator=(WriteGuard&& other) noexcept;

  T* operator->() const;
  T& operator*() const;

 private:
  friend class ObservedAsync<T>;
  friend class AsyncObserver<T>;

  WriteGuard(T* raw_ptr, base::WriteGuard&& rw_lock);

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
ObservedLocal<T>::ObservedLocal(std::nullptr_t) {}

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
  if (is_null_) {
    *is_null_ = true;
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
  return ObservedLocal(new T(std::forward<Args>(args)...));
}

template <typename T>
template <typename T1>
ObservedLocal<T1> ObservedLocal<T>::TryConvert() {
  T1* raw_ptr = dynamic_cast<T1*>(raw_ptr_);
  if (raw_ptr == nullptr) {
    return nullptr;
  }
  return std::move(*this).template Convert<T1>();
}

template <typename T>
template <typename T1>
ObservedLocal<T1> ObservedLocal<T>::Convert() && {
  auto rv =
      ObservedLocal<T1>(static_cast<T1*>(raw_ptr_), is_null_, observer_cnt_);
  ResetPtr();
  return rv;
}

template <typename T>
LocalObserver<T> ObservedLocal<T>::NewObserver() const {
  MIRAGE_DCHECK(raw_ptr_ != nullptr);
  MIRAGE_DCHECK(observer_cnt_ != nullptr);
  observer_cnt_->Increase();
  return LocalObserver<T>(raw_ptr_, is_null_, observer_cnt_);
}

template <typename T>
T* ObservedLocal<T>::operator->() const {
  return raw_ptr_;
}

template <typename T>
T& ObservedLocal<T>::operator*() const {
  return *raw_ptr_;
}

template <typename T>
T* ObservedLocal<T>::raw_ptr() const {
  return raw_ptr_;
}

template <typename T>
ObservedLocal<T>::operator bool() const {
  return !is_null();
}

template <typename T>
bool ObservedLocal<T>::operator==(std::nullptr_t) const {
  return is_null();
}

template <typename T>
bool ObservedLocal<T>::is_null() const {
  return is_null_ == nullptr || *is_null_;
}

template <typename T>
size_t ObservedLocal<T>::observer_cnt() const {
  return observer_cnt_ ? observer_cnt_->cnt() : 0;
}

template <typename T>
ObservedLocal<T>::ObservedLocal(T* raw_ptr, bool* is_null,
                                RefCount* observer_cnt)
    : raw_ptr_(raw_ptr), is_null_(is_null), observer_cnt_(observer_cnt) {}

template <typename T>
void ObservedLocal<T>::ResetPtr() {
  raw_ptr_ = nullptr;
  is_null_ = nullptr;
  observer_cnt_ = nullptr;
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
  other.ResetPtr();
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
LocalObserver<T>::LocalObserver(std::nullptr_t) {}

template <typename T>
LocalObserver<T>& LocalObserver<T>::operator=(std::nullptr_t) {
  Reset();
  return *this;
}

template <typename T>
void LocalObserver<T>::Reset() {
  if (!observer_cnt_) {
    return;
  }
  if (observer_cnt_->TryRelease() && is_null()) {
    delete is_null_;
    delete observer_cnt_;
  }
  ResetPtr();
}

template <typename T>
LocalObserver<T> LocalObserver<T>::Clone() const {
  if (observer_cnt_ != nullptr) {
    observer_cnt_->TryIncrease();
  }
  return LocalObserver(raw_ptr_, is_null_, observer_cnt_);
}

template <typename T>
template <typename T1>
LocalObserver<T1> LocalObserver<T>::TryConvert() {
  T1* raw_ptr = dynamic_cast<T1*>(raw_ptr_);
  if (raw_ptr == nullptr) {
    return nullptr;
  }
  return std::move(*this).template Convert<T1>();
}

template <typename T>
template <typename T1>
LocalObserver<T1> LocalObserver<T>::Convert() && {
  auto rv =
      LocalObserver<T1>(static_cast<T1*>(raw_ptr_), is_null_, observer_cnt_);
  ResetPtr();
  return rv;
}

template <typename T>
T* LocalObserver<T>::operator->() const {
  return raw_ptr_;
}

template <typename T>
T& LocalObserver<T>::operator*() const {
  return *raw_ptr_;
}

template <typename T>
T* LocalObserver<T>::raw_ptr() const {
  return raw_ptr_;
}

template <typename T>
LocalObserver<T>::operator bool() const {
  return !is_null();
}

template <typename T>
bool LocalObserver<T>::operator==(std::nullptr_t) const {
  return is_null();
}

template <typename T>
bool LocalObserver<T>::is_null() const {
  return is_null_ == nullptr || *is_null_;
}

template <typename T>
size_t LocalObserver<T>::observer_cnt() const {
  return observer_cnt_ ? observer_cnt_->cnt() : 0;
}

template <typename T>
LocalObserver<T>::LocalObserver(T* raw_ptr, bool* is_null,
                                RefCount* observer_cnt)
    : raw_ptr_(raw_ptr), is_null_(is_null), observer_cnt_(observer_cnt) {}

template <typename T>
void LocalObserver<T>::ResetPtr() {
  raw_ptr_ = nullptr;
  is_null_ = nullptr;
  observer_cnt_ = nullptr;
}

template <typename T>
ObservedAsync<T>::~ObservedAsync() {
  Reset();
}

template <typename T>
ObservedAsync<T>::ObservedAsync(ObservedAsync&& other) noexcept
    : raw_ptr_(other.raw_ptr_),
      rw_lock_(other.rw_lock_),
      is_null_(other.is_null_),
      observer_cnt_(other.observer_cnt_) {
  other.ResetPtr();
}

template <typename T>
ObservedAsync<T>& ObservedAsync<T>::operator=(ObservedAsync&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~ObservedAsync();
  new (this) ObservedAsync(std::move(other));
  return *this;
}

template <typename T>
ObservedAsync<T>::ObservedAsync(T* raw_ptr)
    : raw_ptr_(raw_ptr),
      rw_lock_(new RWLock()),
      is_null_(new bool(false)),
      observer_cnt_(new RefCountAsync(0)) {
  MIRAGE_DCHECK(raw_ptr != nullptr);
}

template <typename T>
ObservedAsync<T>::ObservedAsync(std::nullptr_t) {}

template <typename T>
ObservedAsync<T>& ObservedAsync<T>::operator=(std::nullptr_t) {
  Reset();
  return *this;
}

template <typename T>
void ObservedAsync<T>::Reset() {
  if (is_null()) {
    return;
  }
  auto guard = base::ScopedWriteGuard(*rw_lock_);
  *is_null_ = true;
  delete raw_ptr_;
  guard.Reset();

  if (observer_cnt_->cnt() == 0) {
    delete rw_lock_;
    delete is_null_;
    delete observer_cnt_;
  }
  ResetPtr();
}

template <typename T>
template <typename... Args>
ObservedAsync<T> ObservedAsync<T>::New(Args&&... args) {
  return ObservedAsync(new T(std::forward<Args>(args)...));
}

template <typename T>
template <typename T1>
ObservedAsync<T1> ObservedAsync<T>::TryConvert() {
  T1* raw_ptr = dynamic_cast<T1*>(raw_ptr_);
  if (raw_ptr == nullptr) {
    return nullptr;
  }
  return std::move(*this).template Convert<T1>();
}

template <typename T>
template <typename T1>
ObservedAsync<T1> ObservedAsync<T>::Convert() && {
  auto rv = ObservedAsync<T1>(static_cast<T1*>(raw_ptr_), rw_lock_, is_null_,
                              observer_cnt_);
  ResetPtr();
  return rv;
}

template <typename T>
AsyncObserver<T> ObservedAsync<T>::NewObserver() const {
  MIRAGE_DCHECK(observer_cnt_ != nullptr);
  observer_cnt_->Increase();
  return AsyncObserver<T>(raw_ptr_, rw_lock_, is_null_, observer_cnt_);
}

template <typename T>
typename ObservedAsync<T>::ReadGuard ObservedAsync<T>::Read() const {
  MIRAGE_DCHECK(rw_lock_ != nullptr);
  return ReadGuard(raw_ptr_, base::ReadGuard(*rw_lock_));
}

template <typename T>
typename ObservedAsync<T>::WriteGuard ObservedAsync<T>::Write() const {
  MIRAGE_DCHECK(rw_lock_ != nullptr);
  return WriteGuard(raw_ptr_, base::WriteGuard(*rw_lock_));
}

template <typename T>
T* ObservedAsync<T>::raw_ptr() const {
  return raw_ptr_;
}

template <typename T>
ObservedAsync<T>::operator bool() const {
  return !is_null();
}

template <typename T>
bool ObservedAsync<T>::operator==(std::nullptr_t) const {
  return is_null();
}

template <typename T>
bool ObservedAsync<T>::is_null() const {
  auto guard = base::ScopedReadGuard(*rw_lock_);
  return is_null_ == nullptr || *is_null_;
}

template <typename T>
size_t ObservedAsync<T>::observer_cnt() const {
  return observer_cnt_ ? observer_cnt_->cnt() : 0;
}

template <typename T>
ObservedAsync<T>::ObservedAsync(T* raw_ptr, RWLock* rw_lock, bool* is_null,
                                RefCount* observer_cnt)
    : raw_ptr_(raw_ptr),
      rw_lock_(rw_lock),
      is_null_(is_null),
      observer_cnt_(observer_cnt) {}

template <typename T>
void ObservedAsync<T>::ResetPtr() {
  raw_ptr_ = nullptr;
  rw_lock_ = nullptr;
  is_null_ = nullptr;
  observer_cnt_ = nullptr;
}

template <typename T>
AsyncObserver<T>::~AsyncObserver() {
  Reset();
}

template <typename T>
AsyncObserver<T>::AsyncObserver(AsyncObserver&& other) noexcept
    : raw_ptr_(other.raw_ptr_),
      rw_lock_(other.rw_lock_),
      is_null_(other.is_null_),
      observer_cnt_(other.observer_cnt_) {
  other.ResetPtr();
}

template <typename T>
AsyncObserver<T>& AsyncObserver<T>::operator=(AsyncObserver&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~AsyncObserver();
  new (this) AsyncObserver(std::move(other));
  return *this;
}

template <typename T>
AsyncObserver<T>::AsyncObserver(std::nullptr_t) {}

template <typename T>
AsyncObserver<T>& AsyncObserver<T>::operator=(std::nullptr_t) {
  Reset();
  return *this;
}

template <typename T>
void AsyncObserver<T>::Reset() {
  if (!observer_cnt_) {
    return;
  }
  if (observer_cnt_->TryRelease() && is_null()) {
    delete rw_lock_;
    delete is_null_;
    delete observer_cnt_;
  }
  ResetPtr();
}

template <typename T>
AsyncObserver<T> AsyncObserver<T>::Clone() const {
  if (observer_cnt_ != nullptr) {
    observer_cnt_->TryIncrease();
  }
  return AsyncObserver(raw_ptr_, rw_lock_, is_null_, observer_cnt_);
}

template <typename T>
template <typename T1>
AsyncObserver<T1> AsyncObserver<T>::TryConvert() {
  T1* raw_ptr = dynamic_cast<T1*>(raw_ptr_);
  if (raw_ptr == nullptr) {
    return nullptr;
  }
  return std::move(*this).template Convert<T1>();
}

template <typename T>
template <typename T1>
AsyncObserver<T1> AsyncObserver<T>::Convert() && {
  auto rv = AsyncObserver<T1>(static_cast<T1*>(raw_ptr_), rw_lock_, is_null_,
                              observer_cnt_);
  ResetPtr();
  return rv;
}

template <typename T>
Optional<typename AsyncObserver<T>::ReadGuard> AsyncObserver<T>::Read() const {
  if (rw_lock_ == nullptr) {
    return Optional<ReadGuard>::None();
  }
  auto guard = base::ReadGuard(*rw_lock_);
  if (is_null_ == nullptr || *is_null_) {
    return Optional<ReadGuard>::None();
  }
  MIRAGE_DCHECK(rw_lock_ != nullptr);
  return Optional<ReadGuard>::New(raw_ptr_, std::move(guard));
}

template <typename T>
Optional<typename AsyncObserver<T>::WriteGuard> AsyncObserver<T>::Write()
    const {
  if (rw_lock_ == nullptr) {
    return Optional<ReadGuard>::None();
  }
  auto guard = base::WriteGuard(*rw_lock_);
  if (is_null_ == nullptr || *is_null_) {
    return Optional<WriteGuard>::None();
  }
  MIRAGE_DCHECK(rw_lock_ != nullptr);
  return Optional<WriteGuard>::New(raw_ptr_, std::move(guard));
}

template <typename T>
T* AsyncObserver<T>::raw_ptr() const {
  return raw_ptr_;
}

template <typename T>
AsyncObserver<T>::operator bool() const {
  return !is_null();
}

template <typename T>
bool AsyncObserver<T>::operator==(std::nullptr_t) const {
  return is_null();
}

template <typename T>
bool AsyncObserver<T>::is_null() const {
  auto guard = base::ScopedReadGuard(*rw_lock_);
  return is_null_ == nullptr || *is_null_;
}

template <typename T>
size_t AsyncObserver<T>::observer_cnt() const {
  return observer_cnt_ ? observer_cnt_->cnt() : 0;
}

template <typename T>
AsyncObserver<T>::AsyncObserver(T* raw_ptr, RWLock* rw_lock, bool* is_null,
                                RefCount* observer_cnt)
    : raw_ptr_(raw_ptr),
      rw_lock_(rw_lock),
      is_null_(is_null),
      observer_cnt_(observer_cnt) {}

template <typename T>
void AsyncObserver<T>::ResetPtr() {
  raw_ptr_ = nullptr;
  rw_lock_ = nullptr;
  is_null_ = nullptr;
  observer_cnt_ = nullptr;
}

template <typename T>
ObservedAsync<T>::ReadGuard::ReadGuard(ReadGuard&& other) noexcept
    : raw_ptr_(other.raw_ptr_), read_guard_(std::move(other.read_guard_)) {
  other.raw_ptr_ = nullptr;
}

template <typename T>
typename ObservedAsync<T>::ReadGuard& ObservedAsync<T>::ReadGuard::operator=(
    ReadGuard&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~ReadGuard();
  new (this) ReadGuard(std::move(other));
  return *this;
}

template <typename T>
const T* ObservedAsync<T>::ReadGuard::operator->() const {
  return raw_ptr_;
}

template <typename T>
const T& ObservedAsync<T>::ReadGuard::operator*() const {
  return *raw_ptr_;
}

template <typename T>
ObservedAsync<T>::ReadGuard::ReadGuard(const T* raw_ptr,
                                       base::ReadGuard&& rw_lock)
    : raw_ptr_(raw_ptr), read_guard_(std::move(rw_lock)) {
  MIRAGE_DCHECK(raw_ptr_ != nullptr);
}

template <typename T>
ObservedAsync<T>::WriteGuard::WriteGuard(WriteGuard&& other) noexcept
    : raw_ptr_(other.raw_ptr_), write_guard_(std::move(other.write_guard_)) {
  other.raw_ptr_ = nullptr;
}

template <typename T>
typename ObservedAsync<T>::WriteGuard& ObservedAsync<T>::WriteGuard::operator=(
    WriteGuard&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~WriteGuard();
  new (this) WriteGuard(std::move(other));
  return *this;
}

template <typename T>
T* ObservedAsync<T>::WriteGuard::operator->() const {
  return raw_ptr_;
}

template <typename T>
T& ObservedAsync<T>::WriteGuard::operator*() const {
  return *raw_ptr_;
}

template <typename T>
ObservedAsync<T>::WriteGuard::WriteGuard(T* raw_ptr, base::WriteGuard&& rw_lock)
    : raw_ptr_(raw_ptr), write_guard_(std::move(rw_lock)) {
  MIRAGE_DCHECK(raw_ptr_ != nullptr);
}

}  // namespace mirage::base

#endif  // MIRAGE_BASE_AUTO_PTR_OBSERVED
