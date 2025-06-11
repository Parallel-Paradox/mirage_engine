#ifndef MIRAGE_BASE_SYNC_LOCK
#define MIRAGE_BASE_SYNC_LOCK

#include "mirage_base/define/export.hpp"

namespace mirage::base {

class MIRAGE_BASE Lock {
 public:
  using NativeHandle = void*;

  Lock();
  ~Lock();

  Lock(const Lock&) = delete;
  Lock& operator=(const Lock&) = delete;

  Lock(Lock&& other) noexcept;
  Lock& operator=(Lock&& other) noexcept;

  [[nodiscard]] bool TryAcquire() const;
  void Acquire() const;
  void Release() const;

 private:
  void AcquireInternal() const;

  NativeHandle native_handle_;
};

class MIRAGE_BASE LockGuard {
 public:
  LockGuard() = default;
  explicit LockGuard(const Lock& lock);
  ~LockGuard();

  LockGuard(const LockGuard&) = delete;
  LockGuard& operator=(const LockGuard&) = delete;

  LockGuard(LockGuard&& other) noexcept;
  LockGuard& operator=(LockGuard&& other) noexcept;

  void Reset();

 private:
  const Lock* lock_{nullptr};
};

class MIRAGE_BASE ScopedLockGuard {
 public:
  ScopedLockGuard() = delete;
  explicit ScopedLockGuard(const Lock& lock);
  ~ScopedLockGuard();

  ScopedLockGuard(const ScopedLockGuard&) = delete;
  ScopedLockGuard(ScopedLockGuard&& other) noexcept = delete;

  void Reset();

 private:
  const Lock& lock_;
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_SYNC_LOCK
