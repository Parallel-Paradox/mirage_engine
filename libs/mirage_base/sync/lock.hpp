#ifndef MIRAGE_BASE_SYNC_LOCK
#define MIRAGE_BASE_SYNC_LOCK

#include "mirage_base/define/export.hpp"

namespace mirage::base {

class MIRAGE_BASE Lock {
 public:
  using NativeHandle = void*;

  Lock();
  Lock(const Lock&) = delete;
  ~Lock();

  [[nodiscard]] bool TryAcquire() const;
  void Acquire() const;
  void Release() const;

 private:
  void AcquireInternal() const;

  NativeHandle native_handle_;
};

class MIRAGE_BASE LockGuard {
 public:
  LockGuard() = delete;
  LockGuard(const LockGuard&) = delete;

  explicit LockGuard(const Lock& lock) : lock_(lock) { lock.Acquire(); }
  ~LockGuard() { Reset(); }

  void Reset() { lock_.Release(); }

 private:
  const Lock& lock_;
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_SYNC_LOCK
