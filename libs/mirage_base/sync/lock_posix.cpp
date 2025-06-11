#include <pthread.h>

#include "mirage_base/define/check.hpp"
#include "mirage_base/sync/lock.hpp"

using namespace mirage::base;

Lock::Lock() {
  auto* handle = new pthread_mutex_t();
  [[maybe_unused]] int32_t rv = pthread_mutex_init(handle, nullptr);
  MIRAGE_DCHECK(rv == 0);
  native_handle_ = static_cast<void*>(handle);
}

Lock::~Lock() {
  auto* handle = static_cast<pthread_mutex_t*>(native_handle_);
  [[maybe_unused]] int32_t rv = pthread_mutex_destroy(handle);
  MIRAGE_DCHECK(rv == 0);
  delete handle;
}

bool Lock::TryAcquire() const {
  auto* handle = static_cast<pthread_mutex_t*>(native_handle_);
  return pthread_mutex_trylock(handle) == 0;
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
  [[maybe_unused]] int32_t rv =
      pthread_mutex_unlock(static_cast<pthread_mutex_t*>(native_handle_));
  MIRAGE_DCHECK(rv == 0);
}

void Lock::AcquireInternal() const {
  [[maybe_unused]] int32_t rv =
      pthread_mutex_lock(static_cast<pthread_mutex_t*>(native_handle_));
  MIRAGE_DCHECK(rv == 0);
}
