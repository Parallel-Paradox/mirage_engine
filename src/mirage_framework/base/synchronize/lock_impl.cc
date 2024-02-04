#include "mirage_framework/base/synchronize/lock_impl.h"

using namespace mirage;

LockImpl::LockImpl() {
#if defined(OS_APPLE)
  pthread_mutex_init(&native_handle_, nullptr);
#endif
}

LockImpl::~LockImpl() {
#if defined(OS_APPLE)
  int rv = pthread_mutex_destroy(&native_handle_);
  MIRAGE_DCHECK(rv == 0);
#endif
}

bool LockImpl::TryAcquire() {
#if defined(OS_APPLE)
  int rv = pthread_mutex_trylock(&native_handle_);
  return rv == 0;
#endif
}

void LockImpl::Acquire() {
#if defined(OS_APPLE)
  int rv = pthread_mutex_lock(&native_handle_);
  MIRAGE_DCHECK(rv == 0);
#endif
}

void LockImpl::Release() {
#if defined(OS_APPLE)
  int rv = pthread_mutex_unlock(&native_handle_);
  MIRAGE_DCHECK(rv == 0);
#endif
}
