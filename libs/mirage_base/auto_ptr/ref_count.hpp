﻿#ifndef MIRAGE_BASE_AUTO_PTR_REF_COUNT
#define MIRAGE_BASE_AUTO_PTR_REF_COUNT

#include <concepts>

#include "mirage_base/define.hpp"
#include "mirage_base/sync/lock.hpp"

namespace mirage::base {

class MIRAGE_BASE RefCount {
 public:
  virtual ~RefCount() = default;
  virtual size_t GetCnt() = 0;
  virtual bool TryIncrease() = 0;
  virtual bool TryRelease() = 0;
};

class MIRAGE_BASE RefCountLocal : public RefCount {
 public:
  RefCountLocal() = default;
  ~RefCountLocal() override = default;
  size_t GetCnt() override;
  bool TryIncrease() override;
  bool TryRelease() override;

 private:
  size_t cnt_{1};
};

class MIRAGE_BASE RefCountAsync final : public RefCountLocal {
 public:
  RefCountAsync() = default;
  ~RefCountAsync() override = default;
  size_t GetCnt() override;
  bool TryIncrease() override;
  bool TryRelease() override;

 private:
  Lock lock_;
};

template <typename R>
concept IsRefCount =
    std::default_initializable<R> && std::derived_from<R, RefCount>;

}  // namespace mirage::base

#endif  // MIRAGE_BASE_AUTO_PTR_REF_COUNT
