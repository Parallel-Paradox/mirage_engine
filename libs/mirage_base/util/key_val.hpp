#ifndef MIRAGE_BASE_UTIL_KEY_VAL
#define MIRAGE_BASE_UTIL_KEY_VAL

#include <concepts>

template <std::move_constructible Key, std::move_constructible Val>
struct KeyVal {
  Key key;
  Val val;

  KeyVal() = delete;
  ~KeyVal() = default;

  KeyVal(const KeyVal&) = delete;

  KeyVal(KeyVal&& other) noexcept
      : key(std::move(other.key)), val(std::move(other.val)) {}

  KeyVal& operator=(KeyVal&& other) noexcept {
    if (this != &other) {
      this->~KeyVal();
      new (this) KeyVal(std::move(other));
    }
    return *this;
  }

  KeyVal(Key&& key, Val&& val) : key(std::move(key)), val(std::move(val)) {}

  struct Ref {
    const Key& key;
    Val& val;

    Ref() = delete;
    ~Ref() = default;

    Ref(const Key& key, Val& val) : key(key), val(val) {}
  };

  Ref GetRef() { return Ref(key, val); }
};

#endif  // MIRAGE_BASE_UTIL_KEY_VAL
