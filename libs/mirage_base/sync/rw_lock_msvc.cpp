#include <windows.h>

#include "mirage_base/define/check.hpp"
#include "mirage_base/sync/rw_lock.hpp"

using namespace mirage::base;

RWLock::RWLock() {
  native_handle_ = new SRWLOCK();
  InitializeSRWLock(static_cast<SRWLOCK*>(native_handle_));
}

RWLock::~RWLock() {
  if (native_handle_ == nullptr) {
    return;
  }
  delete static_cast<SRWLOCK*>(native_handle_);
  native_handle_ = nullptr;
}

bool RWLock::TryRead() const {
  MIRAGE_DCHECK(native_handle_ != nullptr);
  return TryAcquireSRWLockShared(static_cast<SRWLOCK*>(native_handle_));
}

bool RWLock::TryWrite() const {
  MIRAGE_DCHECK(native_handle_ != nullptr);
  return TryAcquireSRWLockExclusive(static_cast<SRWLOCK*>(native_handle_));
}

void RWLock::Read() const {
  // Try the lock first to acquire it cheaply if it's not contended. Try() is
  // cheap on platforms with futex-type locks, as it doesn't call into the
  // kernel.
  if (TryRead()) {
    return;
  }
  ReadInternal();
}

void RWLock::Write() const {
  if (TryWrite()) {
    return;
  }
  WriteInternal();
}

void RWLock::UnlockRead() const {
  MIRAGE_DCHECK(native_handle_ != nullptr);
  ReleaseSRWLockShared(static_cast<SRWLOCK*>(native_handle_));
}

void RWLock::UnlockWrite() const {
  MIRAGE_DCHECK(native_handle_ != nullptr);
  ReleaseSRWLockExclusive(static_cast<SRWLOCK*>(native_handle_));
}

void RWLock::ReadInternal() const {
  MIRAGE_DCHECK(native_handle_ != nullptr);
  AcquireSRWLockShared(static_cast<SRWLOCK*>(native_handle_));
}

void RWLock::WriteInternal() const {
  MIRAGE_DCHECK(native_handle_ != nullptr);
  AcquireSRWLockExclusive(static_cast<SRWLOCK*>(native_handle_));
}
