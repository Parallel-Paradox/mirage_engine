#ifndef MIRAGE_FRAMEWORK_BASE_CONTAINER_HASH_MAP
#define MIRAGE_FRAMEWORK_BASE_CONTAINER_HASH_MAP

#include "mirage_framework/base/container/array.hpp"
#include "mirage_framework/base/container/singly_linked_list.hpp"
#include "mirage_framework/base/util/hash.hpp"
#include "mirage_framework/base/util/optional.hpp"
#include "mirage_framework/define.hpp"

namespace mirage {

template <typename T>
concept HashKeyType =
    std::move_constructible<T> && std::equality_comparable<T> &&
    requires(Hash<T> hasher, T val) {
  { hasher(val) } -> std::same_as<size_t>;
};

template <HashKeyType Key, std::move_constructible Val>
class HashMap {
 public:
  HashMap() = default;

  class Entry {
   public:
    Entry() = delete;
    ~Entry() = default;

    Entry(Key&& key, Val&& val, Hash<Key>& hasher)
        : key_(key), val_(val), hash_(hasher(key)) {}

    const Key& GetKey() const { return key_; }

    const Val& GetVal() const { return val_; }

    Val& GetVal() { return val_; }

    void SetVal(Val&& val) {
      (&val_)->~Val();
      new (&val_) Val(std::move(val));
    }

   private:
    Key key_;
    Val val_;

    size_t hash_;
  };

  uint32_t GetSize() const { return size_; }

  Optional<Val> Insert(Key&& key, Val&& val) {
    // TODO
  }

  Val* Find(const Key& key) {
    // TODO
    return nullptr;
  }

  Optional<Val> Remove(const Key& key) {
    // TODO
  }

 private:
  class CongruenceMod {
   public:
    CongruenceMod() = default;
    CongruenceMod(const CongruenceMod&) = delete;
    ~CongruenceMod() = default;

    CongruenceMod(CongruenceMod&& other)
        : list_(std::move(other.list_)), size_(other.size_) {
      other.size_ = 0;
    }

   private:
    SinglyLinkedList<Entry> list_;
    size_t size_{0};
  };

  void ExtendMod() {
    if (mod_array_.IsEmpty()) {
      mod_array_.SetSize(1);
      return;
    }
    size_t new_mod_len = mod_array_.GetSize() << 1;
    mod_array_.SetSize(new_mod_len);
  }

  Array<CongruenceMod> mod_array_;
  size_t mod_size_limit_{4};

  size_t size_{0};
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_CONTAINER_HASH_MAP
