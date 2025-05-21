#ifndef MIRAGE_BASE_SYNCHRONIZE_LOCK
#define MIRAGE_BASE_SYNCHRONIZE_LOCK

#include "mirage_base/define/export.hpp"
#include "mirage_base/sync/lock_impl.hpp"

namespace mirage::base {

class MIRAGE_BASE Lock {
 public:
  Lock() = default;
  ~Lock() = default;

  [[nodiscard]] bool TryAcquire() const;
  void Acquire() const;
  void Release() const;

 private:
  LockImpl lock_;
};

class MIRAGE_BASE LockGuard {
 public:
  LockGuard() = delete;
  LockGuard(const LockGuard&) = delete;

  explicit LockGuard(Lock& lock);
  ~LockGuard();

 private:
  Lock& lock_;
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_SYNCHRONIZE_LOCK
