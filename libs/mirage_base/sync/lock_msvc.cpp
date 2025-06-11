#include <windows.h>

#include "mirage_base/sync/lock.hpp"

using namespace mirage::base;

void Lock::Acquire() const {
  // Try the lock first to acquire it cheaply if it's not contended. Try() is
  // cheap on platforms with futex-type locks, as it doesn't call into the
  // kernel.
  if (TryAcquire()) {
    return;
  }
  AcquireInternal();
}

Lock::Lock() {
  native_handle_ = new SRWLOCK();
  InitializeSRWLock(static_cast<SRWLOCK*>(native_handle_));
}

Lock::~Lock() { delete static_cast<SRWLOCK*>(native_handle_); }

bool Lock::TryAcquire() const {
  return TryAcquireSRWLockExclusive(static_cast<SRWLOCK*>(native_handle_));
}

void Lock::AcquireInternal() const {
  AcquireSRWLockExclusive(static_cast<SRWLOCK*>(native_handle_));
}

void Lock::Release() const {
  ReleaseSRWLockExclusive(static_cast<SRWLOCK*>(native_handle_));
}
