#ifndef MIRAGE_BASE_SYNCHRONIZE_LOCK
#define MIRAGE_BASE_SYNCHRONIZE_LOCK

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

  explicit LockGuard(Lock& lock) : lock_(lock) { lock.Acquire(); }
  ~LockGuard() { lock_.Release(); }

 private:
  Lock& lock_;
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_SYNCHRONIZE_LOCK
