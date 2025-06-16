#include "mirage_base/sync/rw_lock.hpp"

#include <new>  // NOLINT: Need for placement new on mac
#include <utility>

using namespace mirage::base;

RWLock::RWLock(RWLock&& other) noexcept : native_handle_(other.native_handle_) {
  other.native_handle_ = nullptr;
}

RWLock& RWLock::operator=(RWLock&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~RWLock();
  new (this) RWLock(std::move(other));
  return *this;
}

ReadGuard::ReadGuard(const RWLock& rw_lock) : rw_lock_(&rw_lock) {
  rw_lock.Read();
}

ReadGuard::~ReadGuard() { Reset(); }

ReadGuard::ReadGuard(ReadGuard&& other) noexcept : rw_lock_(other.rw_lock_) {
  other.rw_lock_ = nullptr;
}

ReadGuard& ReadGuard::operator=(ReadGuard&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~ReadGuard();
  new (this) ReadGuard(std::move(other));
  return *this;
}

void ReadGuard::Reset() {
  if (!rw_lock_) {
    return;
  }
  rw_lock_->UnlockRead();
  rw_lock_ = nullptr;
}

ScopedReadGuard::ScopedReadGuard(const RWLock& rw_lock) : rw_lock_(rw_lock) {
  rw_lock.Read();
}

ScopedReadGuard::~ScopedReadGuard() { rw_lock_.UnlockRead(); }

WriteGuard::WriteGuard(const RWLock& rw_lock) : rw_lock_(&rw_lock) {
  rw_lock.Write();
}

WriteGuard::~WriteGuard() { Reset(); }

WriteGuard::WriteGuard(WriteGuard&& other) noexcept : rw_lock_(other.rw_lock_) {
  other.rw_lock_ = nullptr;
}

WriteGuard& WriteGuard::operator=(WriteGuard&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~WriteGuard();
  new (this) WriteGuard(std::move(other));
  return *this;
}

void WriteGuard::Reset() {
  if (!rw_lock_) {
    return;
  }
  rw_lock_->UnlockWrite();
  rw_lock_ = nullptr;
}

ScopedWriteGuard::ScopedWriteGuard(const RWLock& rw_lock) : rw_lock_(rw_lock) {
  rw_lock.Write();
}

ScopedWriteGuard::~ScopedWriteGuard() { rw_lock_.UnlockWrite(); }
