#include <pthread.h>

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
  auto* handle = new pthread_mutex_t();
  pthread_mutex_init(handle, nullptr);
  native_handle_ = static_cast<void*>(handle);
}

Lock::~Lock() {
  auto* handle = static_cast<pthread_mutex_t*>(native_handle_);
  pthread_mutex_destroy(handle);
  delete handle;
}

bool Lock::TryAcquire() const {
  auto* handle = static_cast<pthread_mutex_t*>(native_handle_);
  return pthread_mutex_trylock(handle) == 0;
}

void Lock::AcquireInternal() const {
  pthread_mutex_lock(static_cast<pthread_mutex_t*>(native_handle_));
}

void Lock::Release() const {
  pthread_mutex_unlock(static_cast<pthread_mutex_t*>(native_handle_));
}
