#include <pthread.h>

#include "mirage_base/define/check.hpp"
#include "mirage_base/sync/rw_lock.hpp"

using namespace mirage::base;

RWLock::RWLock() {
  auto* handle = new pthread_rwlock_t();
  [[maybe_unused]] int32_t rv = pthread_rwlock_init(handle, nullptr);
  MIRAGE_DCHECK(rv == 0);
  native_handle_ = static_cast<void*>(handle);
}

RWLock::~RWLock() {
  if (native_handle_ == nullptr) {
    return;
  }
  auto* handle = static_cast<pthread_rwlock_t*>(native_handle_);
  [[maybe_unused]] int32_t rv = pthread_rwlock_destroy(handle);
  MIRAGE_DCHECK(rv == 0);
  delete handle;
  native_handle_ = nullptr;
}

bool RWLock::TryRead() const {
  MIRAGE_DCHECK(native_handle_ != nullptr);
  auto* handle = static_cast<pthread_rwlock_t*>(native_handle_);
  return pthread_rwlock_tryrdlock(handle) == 0;
}

bool RWLock::TryWrite() const {
  MIRAGE_DCHECK(native_handle_ != nullptr);
  auto* handle = static_cast<pthread_rwlock_t*>(native_handle_);
  return pthread_rwlock_trywrlock(handle) == 0;
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
  [[maybe_unused]] int32_t rv =
      pthread_rwlock_unlock(static_cast<pthread_rwlock_t*>(native_handle_));
  MIRAGE_DCHECK(rv == 0);
}

void RWLock::UnlockWrite() const {
  MIRAGE_DCHECK(native_handle_ != nullptr);
  [[maybe_unused]] int32_t rv =
      pthread_rwlock_unlock(static_cast<pthread_rwlock_t*>(native_handle_));
  MIRAGE_DCHECK(rv == 0);
}

void RWLock::ReadInternal() const {
  MIRAGE_DCHECK(native_handle_ != nullptr);
  [[maybe_unused]] int32_t rv =
      pthread_rwlock_rdlock(static_cast<pthread_rwlock_t*>(native_handle_));
  MIRAGE_DCHECK(rv == 0);
}

void RWLock::WriteInternal() const {
  MIRAGE_DCHECK(native_handle_ != nullptr);
  [[maybe_unused]] int32_t rv =
      pthread_rwlock_wrlock(static_cast<pthread_rwlock_t*>(native_handle_));
  MIRAGE_DCHECK(rv == 0);
}
