#ifndef MIRAGE_BASE_SYNC_RW_LOCK
#define MIRAGE_BASE_SYNC_RW_LOCK

#include "mirage_base/define/export.hpp"

namespace mirage::base {

class MIRAGE_BASE RWLock {
 public:
  using NativeHandle = void*;

  RWLock();
  RWLock(const RWLock&) = delete;
  ~RWLock();

  [[nodiscard]] bool TryRead() const;
  [[nodiscard]] bool TryWrite() const;
  void Read() const;
  void Write() const;
  void UnlockRead() const;
  void UnlockWrite() const;

 private:
  void ReadInternal() const;
  void WriteInternal() const;

  NativeHandle native_handle_;
};

class MIRAGE_BASE ReadGuard {
 public:
  ReadGuard() = delete;
  ReadGuard(const ReadGuard&) = delete;

  explicit ReadGuard(const RWLock& rw_lock) : rw_lock_(rw_lock) {
    rw_lock_.Read();
  }
  ~ReadGuard() { Reset(); }

  void Reset() { rw_lock_.UnlockRead(); }

 private:
  const RWLock& rw_lock_;
};

class MIRAGE_BASE WriteGuard {
 public:
  WriteGuard() = delete;
  WriteGuard(const WriteGuard&) = delete;

  explicit WriteGuard(const RWLock& rw_lock) : rw_lock_(rw_lock) {
    rw_lock_.Write();
  }
  ~WriteGuard() { Reset(); }

  void Reset() { rw_lock_.UnlockWrite(); }

 private:
  const RWLock& rw_lock_;
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_SYNC_RW_LOCK
