#include "mirage_base/sync/lock.hpp"

#include <utility>

using namespace mirage::base;

Lock::Lock(Lock&& other) noexcept : native_handle_(other.native_handle_) {
  other.native_handle_ = nullptr;
}

Lock& Lock::operator=(Lock&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~Lock();
  new (this) Lock(std::move(other));
  return *this;
}

LockGuard::LockGuard(const Lock& lock) : lock_(&lock) { lock.Acquire(); }

LockGuard::~LockGuard() { Reset(); }

LockGuard::LockGuard(LockGuard&& other) noexcept : lock_(other.lock_) {
  other.lock_ = nullptr;
}

LockGuard& LockGuard::operator=(LockGuard&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~LockGuard();
  new (this) LockGuard(std::move(other));
  return *this;
}

void LockGuard::Reset() {
  if (!lock_) {
    return;
  }
  lock_->Release();
  lock_ = nullptr;
}

ScopedLockGuard::ScopedLockGuard(const Lock& lock) : lock_(lock) {
  lock.Acquire();
}

ScopedLockGuard::~ScopedLockGuard() { Reset(); }

void ScopedLockGuard::Reset() { lock_.Release(); }
