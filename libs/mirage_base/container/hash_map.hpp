#ifndef MIRAGE_BASE_CONTAINER_HASH_MAP
#define MIRAGE_BASE_CONTAINER_HASH_MAP

#include "mirage_base/container/hash_set.hpp"
#include "mirage_base/util/key_val.hpp"

namespace mirage::base {

template <typename T>
concept HashMapKeyType = std::move_constructible<T> && HashType<T>;

template <HashMapKeyType Key, std::move_constructible Val>
struct HashKeyVal : KeyVal<Key, Val> {
  HashKeyVal(Key&& key, Val&& val)
      : KeyVal<Key, Val>(std::move(key), std::move(val)) {}

  bool operator==(const HashKeyVal& other) const {
    return this->key == other.key;
  }

  bool operator==(const Key& other_key) const { return this->key == other_key; }
};

template <HashMapKeyType Key, std::move_constructible Val>
struct Hash<HashKeyVal<Key, Val>> {
  using HashKeyVal = HashKeyVal<Key, Val>;

  Hash<Key> hasher_;

  Hash() = default;
  ~Hash() = default;

  Hash(const Hash&) = default;
  Hash(Hash&&) = default;
  Hash& operator=(const Hash&) = default;
  Hash& operator=(Hash&&) = default;

  size_t operator()(const HashKeyVal& kv) const { return hasher_(kv.key); }
  size_t operator()(const Key& key) const { return hasher_(key); }
};

template <HashMapKeyType Key, std::move_constructible Val>
class HashMap {
 public:
  using KeyValSet = HashSet<HashKeyVal<Key, Val>>;

  class ConstIterator;
  class Iterator;

  HashMap() = default;
  ~HashMap() = default;

  HashMap(HashMap&& other) = default;
  HashMap& operator=(HashMap&& other) = default;

  HashMap(const HashMap&) = delete;
  HashMap& operator=(const HashMap&) = delete;

  HashMap(std::initializer_list<HashKeyVal<Key, Val>> list)
    requires std::copy_constructible<Key> && std::copy_constructible<Val>;

  ConstIterator TryFind(const Key& key) const;
  Iterator TryFind(const Key& key);

  [[nodiscard]] bool IsEmpty() const;
  [[nodiscard]] size_t GetSize() const;

 private:
  KeyValSet kv_set_;
};

template <HashMapKeyType Key, std::move_constructible Val>
class HashMap<Key, Val>::ConstIterator {
 public:
  using iterator_concept = std::forward_iterator_tag;
  using iterator_category = std::forward_iterator_tag;
  using iterator_type = ConstIterator;
  using difference_type = int64_t;
  using value_type = const HashKeyVal<Key, Val>;
  using pointer = value_type*;
  using reference = value_type&;

  ConstIterator() = default;
  ~ConstIterator() = default;

  ConstIterator(const ConstIterator&) = default;
  ConstIterator(ConstIterator&&) = default;

  ConstIterator(std::nullptr_t);        // NOLINT: Convert from nullptr
  ConstIterator(const Iterator& iter);  // NOLINT: Convert to const

  iterator_type& operator=(const iterator_type& other) = default;
  iterator_type& operator=(iterator_type&& other) = default;
  iterator_type& operator=(std::nullptr_t);
  reference operator*() const;
  pointer operator->() const;
  iterator_type& operator++();
  iterator_type operator++(int);
  bool operator==(const iterator_type& other) const;
  bool operator==(std::nullptr_t) const;
  explicit operator bool() const;

 private:
  friend class HashMap;

  explicit ConstIterator(typename KeyValSet::ConstIterator kv_iter)
      : kv_iter_(kv_iter) {}

  typename KeyValSet::ConstIterator kv_iter_;
};

template <HashMapKeyType Key, std::move_constructible Val>
class HashMap<Key, Val>::Iterator {
 public:
  using iterator_concept = std::forward_iterator_tag;
  using iterator_category = std::forward_iterator_tag;
  using iterator_type = Iterator;
  using difference_type = int64_t;
  using value_type = HashKeyVal<Key, Val>;
  using pointer = value_type*;
  using reference = value_type&;

  Iterator() = default;
  ~Iterator() = default;

  Iterator(const Iterator&) = default;
  Iterator(Iterator&&) = default;

  Iterator(std::nullptr_t);  // NOLINT: Convert from nullptr

  iterator_type& operator=(const iterator_type& other) = default;
  iterator_type& operator=(iterator_type&& other) = default;
  iterator_type& operator=(std::nullptr_t);
  reference operator*() const;
  pointer operator->() const;
  iterator_type& operator++();
  iterator_type operator++(int);
  bool operator==(const iterator_type& other) const;
  bool operator==(std::nullptr_t) const;
  explicit operator bool() const;

 private:
  friend class HashMap;

  explicit Iterator(typename KeyValSet::Iterator kv_iter) : kv_iter_(kv_iter) {}

  typename KeyValSet::Iterator kv_iter_;
};

template <HashMapKeyType Key, std::move_constructible Val>
HashMap<Key, Val>::HashMap(std::initializer_list<HashKeyVal<Key, Val>> list)
  requires std::copy_constructible<Key> && std::copy_constructible<Val>
{
  for (const auto& kv : list) {
    kv_set_.Insert({Key(kv.key), Val(kv.val)});
  }
}

template <HashMapKeyType Key, std::move_constructible Val>
typename HashMap<Key, Val>::ConstIterator HashMap<Key, Val>::TryFind(
    const Key& key) const {
  return ConstIterator(kv_set_.TryFind(key));
}

template <HashMapKeyType Key, std::move_constructible Val>
typename HashMap<Key, Val>::Iterator HashMap<Key, Val>::TryFind(
    const Key& key) {
  return Iterator(kv_set_.TryFind(key));
}

template <HashMapKeyType Key, std::move_constructible Val>
bool HashMap<Key, Val>::IsEmpty() const {
  return kv_set_.IsEmpty();
}

template <HashMapKeyType Key, std::move_constructible Val>
size_t HashMap<Key, Val>::GetSize() const {
  return kv_set_.GetSize();
}

template <HashMapKeyType Key, std::move_constructible Val>
HashMap<Key, Val>::ConstIterator::ConstIterator(std::nullptr_t)
    : kv_iter_(nullptr) {}

template <HashMapKeyType Key, std::move_constructible Val>
HashMap<Key, Val>::ConstIterator::ConstIterator(const Iterator& iter)
    : kv_iter_(iter.kv_iter_) {}

template <HashMapKeyType Key, std::move_constructible Val>
typename HashMap<Key, Val>::ConstIterator::iterator_type&
HashMap<Key, Val>::ConstIterator::operator=(std::nullptr_t) {
  kv_iter_ = nullptr;
  return *this;
}

template <HashMapKeyType Key, std::move_constructible Val>
typename HashMap<Key, Val>::ConstIterator::reference
HashMap<Key, Val>::ConstIterator::operator*() const {
  return *kv_iter_;
}

template <HashMapKeyType Key, std::move_constructible Val>
typename HashMap<Key, Val>::ConstIterator::pointer
HashMap<Key, Val>::ConstIterator::operator->() const {
  return kv_iter_.operator->();
}

template <HashMapKeyType Key, std::move_constructible Val>
typename HashMap<Key, Val>::ConstIterator::iterator_type&
HashMap<Key, Val>::ConstIterator::operator++() {
  ++kv_iter_;
  return *this;
}

template <HashMapKeyType Key, std::move_constructible Val>
typename HashMap<Key, Val>::ConstIterator::iterator_type
HashMap<Key, Val>::ConstIterator::operator++(int) {
  iterator_type rv = *this;
  ++(*this);
  return rv;
}

template <HashMapKeyType Key, std::move_constructible Val>
bool HashMap<Key, Val>::ConstIterator::operator==(
    const iterator_type& other) const {
  return kv_iter_ == other.kv_iter_;
}

template <HashMapKeyType Key, std::move_constructible Val>
bool HashMap<Key, Val>::ConstIterator::operator==(std::nullptr_t) const {
  return kv_iter_ == nullptr;
}

template <HashMapKeyType Key, std::move_constructible Val>
HashMap<Key, Val>::ConstIterator::operator bool() const {
  return kv_iter_ != nullptr;
}

template <HashMapKeyType Key, std::move_constructible Val>
HashMap<Key, Val>::Iterator::Iterator(std::nullptr_t) : kv_iter_(nullptr) {}

template <HashMapKeyType Key, std::move_constructible Val>
typename HashMap<Key, Val>::Iterator::iterator_type&
HashMap<Key, Val>::Iterator::operator=(std::nullptr_t) {
  kv_iter_ = nullptr;
  return *this;
}

template <HashMapKeyType Key, std::move_constructible Val>
typename HashMap<Key, Val>::Iterator::reference
HashMap<Key, Val>::Iterator::operator*() const {
  return const_cast<reference>(*kv_iter_);
}

template <HashMapKeyType Key, std::move_constructible Val>
typename HashMap<Key, Val>::Iterator::pointer
HashMap<Key, Val>::Iterator::operator->() const {
  return const_cast<pointer>(kv_iter_.operator->());
}

template <HashMapKeyType Key, std::move_constructible Val>
typename HashMap<Key, Val>::Iterator::iterator_type&
HashMap<Key, Val>::Iterator::operator++() {
  ++kv_iter_;
  return *this;
}

template <HashMapKeyType Key, std::move_constructible Val>
typename HashMap<Key, Val>::Iterator::iterator_type
HashMap<Key, Val>::Iterator::operator++(int) {
  iterator_type rv = *this;
  ++(*this);
  return rv;
}

template <HashMapKeyType Key, std::move_constructible Val>
bool HashMap<Key, Val>::Iterator::operator==(const iterator_type& other) const {
  return kv_iter_ == other.kv_iter_;
}

template <HashMapKeyType Key, std::move_constructible Val>
bool HashMap<Key, Val>::Iterator::operator==(std::nullptr_t) const {
  return kv_iter_ == nullptr;
}

template <HashMapKeyType Key, std::move_constructible Val>
HashMap<Key, Val>::Iterator::operator bool() const {
  return kv_iter_ != nullptr;
}

}  // namespace mirage::base

#endif  // MIRAGE_BASE_CONTAINER_HASH_MAP
