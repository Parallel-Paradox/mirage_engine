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

  KeyVal(KeyVal&& other) noexcept = default;
  KeyVal& operator=(KeyVal&& other) noexcept = default;

  KeyVal(Key&& key, Val&& val) : key(std::move(key)), val(std::move(val)) {}
};

#endif  // MIRAGE_BASE_UTIL_KEY_VAL
