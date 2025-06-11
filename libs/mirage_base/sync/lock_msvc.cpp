#include <windows.h>

#include "mirage_base/define/check.hpp"
#include "mirage_base/sync/lock.hpp"

using namespace mirage::base;

Lock::Lock() {
  native_handle_ = new SRWLOCK();
  InitializeSRWLock(static_cast<SRWLOCK*>(native_handle_));
}

Lock::~Lock() {
  if (native_handle_ == nullptr) {
    return;
  }
  delete static_cast<SRWLOCK*>(native_handle_);
  native_handle_ = nullptr;
}

bool Lock::TryAcquire() const {
  MIRAGE_DCHECK(native_handle_ != nullptr);
  return TryAcquireSRWLockExclusive(static_cast<SRWLOCK*>(native_handle_));
}

void Lock::Acquire() const {
  // Try the lock first to acquire it cheaply if it's not contended. Try() is
  // cheap on platforms with futex-type locks, as it doesn't call into the
  // kernel.
  if (TryAcquire()) {
    return;
  }
  AcquireInternal();
}

void Lock::Release() const {
  MIRAGE_DCHECK(native_handle_ != nullptr);
  ReleaseSRWLockExclusive(static_cast<SRWLOCK*>(native_handle_));
}

void Lock::AcquireInternal() const {
  MIRAGE_DCHECK(native_handle_ != nullptr);
  AcquireSRWLockExclusive(static_cast<SRWLOCK*>(native_handle_));
}
