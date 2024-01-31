#include "mirage_framework/base/auto_ptr/ref_count.hpp"

using namespace mirage;

size_t RefCountLocal::GetCnt() {
  return cnt_;
}

void RefCountLocal::Increase() {
  cnt_ += 1;
}

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

size_t RefCountAsync::GetCnt() {
  std::lock_guard<std::mutex> lock(lock_);
  return RefCountLocal::GetCnt();
}

void RefCountAsync::Increase() {
  std::lock_guard<std::mutex> lock(lock_);
  RefCountLocal::Increase();
}

bool RefCountAsync::TryIncrease() {
  std::lock_guard<std::mutex> lock(lock_);
  return RefCountLocal::TryIncrease();
}

bool RefCountAsync::TryRelease() {
  std::lock_guard<std::mutex> lock(lock_);
  return RefCountLocal::TryRelease();
}
