#ifndef MIRAGE_FRAMEWORK_BASE_SYNCHRONIZE_LOCK_IMPL
#define MIRAGE_FRAMEWORK_BASE_SYNCHRONIZE_LOCK_IMPL

#include "mirage_framework/define.hpp"

#if defined(OS_APPLE)
#include <pthread.h>
#endif

namespace mirage {

class MIRAGE_API LockImpl {
 public:
#if defined(OS_APPLE)
  using NativeHandle = pthread_mutex_t;
#elif defined(OS_WIN)
  using NativeHandle = void*;
#endif

  LockImpl();
  LockImpl(const LockImpl&) = delete;
  ~LockImpl();

  bool TryAcquire();
  void Acquire();
  void Release();

 private:
  void AcquireInternal();

  NativeHandle native_handle_;
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_SYNCHRONIZE_LOCK_IMPL
