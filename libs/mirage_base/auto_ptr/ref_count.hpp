#ifndef MIRAGE_BASE_AUTO_PTR_REF_COUNT
#define MIRAGE_BASE_AUTO_PTR_REF_COUNT

#include <concepts>

#include "mirage_base/define/export.hpp"
#include "mirage_base/sync/lock.hpp"

namespace mirage::base {

class MIRAGE_BASE RefCount {
 public:
  virtual ~RefCount() = default;
  [[nodiscard]] virtual size_t cnt() const = 0;
  virtual void set_cnt(size_t cnt) = 0;
  virtual bool TryIncrease() = 0;
  virtual bool TryRelease() = 0;
};

class MIRAGE_BASE RefCountLocal : public RefCount {
 public:
  explicit RefCountLocal(size_t cnt);
  ~RefCountLocal() override = default;
  [[nodiscard]] size_t cnt() const override;
  void set_cnt(size_t cnt) override;
  bool TryIncrease() override;
  bool TryRelease() override;

 private:
  size_t cnt_{0};
};

class MIRAGE_BASE RefCountAsync final : public RefCountLocal {
 public:
  explicit RefCountAsync(size_t cnt);
  ~RefCountAsync() override = default;
  [[nodiscard]] size_t cnt() const override;
  void set_cnt(size_t cnt) override;
  bool TryIncrease() override;
  bool TryRelease() override;

 private:
  mutable Lock lock_;
};

template <typename R>
concept IsRefCount = std::derived_from<R, RefCount> && requires(R r) {
  { new R(0) } -> std::same_as<R*>;
};

}  // namespace mirage::base

#endif  // MIRAGE_BASE_AUTO_PTR_REF_COUNT
