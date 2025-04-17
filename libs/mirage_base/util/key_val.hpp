#ifndef MIRAGE_BASE_UTIL_KEY_VAL
#define MIRAGE_BASE_UTIL_KEY_VAL

#include <concepts>
#include <type_traits>

template <typename Key, std::move_constructible Val>
  requires std::move_constructible<std::remove_const_t<Key>>
class KeyVal {
 public:
  using KeyType = std::remove_const_t<Key>;

  KeyVal() = delete;
  ~KeyVal() = default;

  KeyVal(const KeyVal&) = delete;

  KeyVal(KeyVal&& other) noexcept = default;
  KeyVal& operator=(KeyVal&& other) noexcept = default;

  KeyVal(KeyType&& key, Val&& val)
      : key_(std::move(key)), val_(std::move(val)) {}

  [[nodiscard]] const KeyType& key() const { return key_; }
  [[nodiscard]] const Val& val() const { return val_; }

  KeyType& key()
    requires std::same_as<Key, KeyType>
  {
    return key_;
  }

  Val& val() { return val_; }

 private:
  KeyType key_;
  Val val_;
};

#endif  // MIRAGE_BASE_UTIL_KEY_VAL
