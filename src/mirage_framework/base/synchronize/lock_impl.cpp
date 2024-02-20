#include "mirage_framework/base/synchronize/lock_impl.hpp"

#if defined(OS_WIN)
#include <windows.h>
#endif

using namespace mirage;

void LockImpl::Acquire() {
  // Try the lock first to acquire it cheaply if it's not contended. Try() is
  // cheap on platforms with futex-type locks, as it doesn't call into the
  // kernel.
  if (TryAcquire()) {
    return;
  }
  AcquireInternal();
}

#if defined(OS_APPLE)

LockImpl::LockImpl() {
  pthread_mutex_init(&native_handle_, nullptr);
}

LockImpl::~LockImpl() {
  int rv = pthread_mutex_destroy(&native_handle_);
  MIRAGE_DCHECK(rv == 0);
}

bool LockImpl::TryAcquire() {
  int rv = pthread_mutex_trylock(&native_handle_);
  return rv == 0;
}

void LockImpl::AcquireInternal() {
  int rv = pthread_mutex_lock(&native_handle_);
  MIRAGE_DCHECK(rv == 0);
}

void LockImpl::Release() {
  int rv = pthread_mutex_unlock(&native_handle_);
  MIRAGE_DCHECK(rv == 0);
}

#elif defined(OS_WIN)

LockImpl::LockImpl() {
  native_handle_ = new SRWLOCK();
  InitializeSRWLock(reinterpret_cast<SRWLOCK*>(native_handle_));
}

LockImpl::~LockImpl() = default;

bool LockImpl::TryAcquire() {
  return TryAcquireSRWLockExclusive(reinterpret_cast<SRWLOCK*>(native_handle_));
}

void LockImpl::AcquireInternal() {
  AcquireSRWLockExclusive(reinterpret_cast<SRWLOCK*>(native_handle_));
}

void LockImpl::Release() {
  ReleaseSRWLockExclusive(reinterpret_cast<SRWLOCK*>(native_handle_));
}

#endif
