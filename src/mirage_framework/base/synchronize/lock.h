#ifndef MIRAGE_FRAMEWORK_BASE_SYNCHRONIZE_LOCK
#define MIRAGE_FRAMEWORK_BASE_SYNCHRONIZE_LOCK

#include "mirage_framework/base/synchronize/lock_impl.h"
#include "mirage_framework/define.hpp"

namespace mirage {

class MIRAGE_API Lock {
 public:
  Lock() = default;
  ~Lock() = default;

  bool TryAcquire();
  void Acquire();
  void Release();

 private:
  LockImpl lock_;
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_SYNCHRONIZE_LOCK
