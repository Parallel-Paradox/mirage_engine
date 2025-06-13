#ifndef MIRAGE_BASE_SYNC_RW_LOCK
#define MIRAGE_BASE_SYNC_RW_LOCK

#include "mirage_base/define/export.hpp"

namespace mirage::base {

class MIRAGE_BASE RWLock {
 public:
  using NativeHandle = void*;

  RWLock();
  ~RWLock();

  RWLock(const RWLock&) = delete;
  RWLock& operator=(const RWLock&) = delete;

  RWLock(RWLock&& other) noexcept;
  RWLock& operator=(RWLock&& other) noexcept;

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
  explicit ReadGuard(const RWLock& rw_lock);
  ~ReadGuard();

  ReadGuard(const ReadGuard&) = delete;
  ReadGuard& operator=(const ReadGuard&) = delete;

  ReadGuard(ReadGuard&& other) noexcept;
  ReadGuard& operator=(ReadGuard&& other) noexcept;

  void Reset();

 private:
  const RWLock* rw_lock_;
};

class MIRAGE_BASE ScopedReadGuard {
 public:
  ScopedReadGuard() = delete;
  explicit ScopedReadGuard(const RWLock& rw_lock);
  ~ScopedReadGuard();

  ScopedReadGuard(const ScopedReadGuard&) = delete;
  ScopedReadGuard(ScopedReadGuard&& other) noexcept = delete;

 private:
  const RWLock& rw_lock_;
};

class MIRAGE_BASE WriteGuard {
 public:
  WriteGuard() = delete;
  explicit WriteGuard(const RWLock& rw_lock);
  ~WriteGuard();

  WriteGuard(const WriteGuard&) = delete;
  WriteGuard& operator=(const WriteGuard&) = delete;

  WriteGuard(WriteGuard&& other) noexcept;
  WriteGuard& operator=(WriteGuard&& other) noexcept;

  void Reset();

 private:
  const RWLock* rw_lock_;
};

class MIRAGE_BASE ScopedWriteGuard {
 public:
  ScopedWriteGuard() = delete;
  explicit ScopedWriteGuard(const RWLock& rw_lock);
  ~ScopedWriteGuard();

  ScopedWriteGuard(const ScopedWriteGuard&) = delete;
  ScopedWriteGuard(ScopedWriteGuard&& other) noexcept = delete;

 private:
  const RWLock& rw_lock_;
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_SYNC_RW_LOCK
