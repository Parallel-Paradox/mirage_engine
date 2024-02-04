#include "mirage_framework/base/synchronize/lock.h"

using namespace mirage;

bool Lock::TryAcquire() {
  return lock_.TryAcquire();
}

void Lock::Acquire() {
  lock_.Acquire();
}

void Lock::Release() {
  lock_.Release();
}
