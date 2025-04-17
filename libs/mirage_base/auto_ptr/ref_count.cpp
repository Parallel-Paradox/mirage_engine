#include "mirage_base/auto_ptr/ref_count.hpp"

using namespace mirage::base;

RefCountLocal::RefCountLocal(const size_t cnt) : cnt_(cnt) {}

size_t RefCountLocal::cnt() const { return cnt_; }

void RefCountLocal::set_cnt(const size_t cnt) { cnt_ = cnt; }

bool RefCountLocal::TryIncrease() {
  if (cnt_ == 0) {
    return false;
  }
  cnt_ += 1;
  return true;
}

bool RefCountLocal::TryRelease() {
  if (cnt_ == 0) {
    return true;
  }
  cnt_ -= 1;
  return cnt_ == 0;
}

RefCountAsync::RefCountAsync(const size_t cnt) : RefCountLocal(cnt) {}

size_t RefCountAsync::cnt() const {
  LockGuard lock(lock_);
  return RefCountLocal::cnt();
}

void RefCountAsync::set_cnt(const size_t cnt) {
  LockGuard lock(lock_);
  RefCountLocal::set_cnt(cnt);
}

bool RefCountAsync::TryIncrease() {
  LockGuard lock(lock_);
  return RefCountLocal::TryIncrease();
}

bool RefCountAsync::TryRelease() {
  LockGuard lock(lock_);
  return RefCountLocal::TryRelease();
}
