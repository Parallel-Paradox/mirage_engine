#ifndef MIRAGE_BASE_CONTAINER_HASH_SET
#define MIRAGE_BASE_CONTAINER_HASH_SET

#include <concepts>
#include <initializer_list>
#include <iterator>

#include "mirage_base/container/array.hpp"
#include "mirage_base/container/singly_linked_list.hpp"
#include "mirage_base/util/hash.hpp"
#include "mirage_base/util/optional.hpp"

namespace mirage::base {

template <typename T>
concept HashSetValType = std::move_constructible<T> && HashType<T>;

template <HashSetValType T>
class HashSet {
 public:
  class ConstIterator;

  HashSet() = default;
  ~HashSet() = default;

  HashSet(std::initializer_list<T> list)
    requires std::copy_constructible<T>;

  Optional<T> Insert(T val);
  Optional<T> Remove(const T& val);

  ConstIterator TryFind(const T& val) const;

  void Clear();
  [[nodiscard]] bool IsEmpty() const;
  [[nodiscard]] size_t GetSize() const;

  [[nodiscard]] float GetMaxLoadFactor() const;
  void SetMaxLoadFactor(float load_factor);

  [[nodiscard]] size_t GetBucketSize() const;

  ConstIterator begin() const;
  ConstIterator end() const;

 private:
  struct Entry {
    T val;
    size_t hash;
  };

  void ExtendAndRehash();

  Hash<T> hasher_;
  Array<SinglyLinkedList<Entry>> buckets_;
  size_t size_ = 0;
  float max_load_factor_ = 1.0;
};

template <HashSetValType T>
class HashSet<T>::ConstIterator {
 public:
  using iterator_concept = std::forward_iterator_tag;
  using iterator_category = std::forward_iterator_tag;
  using iterator_type = ConstIterator;
  using difference_type = int64_t;
  using value_type = const T;
  using pointer = value_type*;
  using reference = value_type&;

  using BucketIter = typename Array<SinglyLinkedList<Entry>>::ConstIterator;
  using EntryIter = typename SinglyLinkedList<Entry>::ConstIterator;

  ConstIterator() = default;
  ~ConstIterator() = default;

  ConstIterator(const ConstIterator&) = default;
  ConstIterator(ConstIterator&&) = default;

  ConstIterator(BucketIter bucket_iter, BucketIter bucket_end,
                EntryIter entry_iter);

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
  BucketIter bucket_iter_;
  BucketIter bucket_end_;
  EntryIter entry_iter_;
};

template <HashSetValType T>
HashSet<T>::HashSet(std::initializer_list<T> list)
  requires std::copy_constructible<T>
{
  for (const auto& val : list) {
    Insert(T(val));
  }
}

template <HashSetValType T>
Optional<T> HashSet<T>::Insert(T val) {
  ConstIterator iter = TryFind(val);
  if (iter != end()) {
    auto rv = Optional<T>::New(std::move(const_cast<T&>(*iter)));
    new (const_cast<T*>(iter.operator->())) T(std::move(val));
    return rv;
  }

  ++size_;
  if (buckets_.IsEmpty()) {
    buckets_.SetSize(16);
  }
  if (const float load_factor = static_cast<float>(size_) / buckets_.GetSize();
      load_factor > max_load_factor_) {
    ExtendAndRehash();
  }

  const size_t hash = hasher_(val);
  const size_t mask = buckets_.GetSize() - 1;
  auto& bucket = buckets_[hash & mask];
  bucket.EmplaceHead(Entry{std::move(val), hash});
  return Optional<T>::None();
}

template <HashSetValType T>
Optional<T> HashSet<T>::Remove(const T& val) {
  if (size_ == 0) {
    return Optional<T>::None();
  }
  const size_t hash = hasher_(val);
  const size_t mask = buckets_.GetSize() - 1;
  auto& bucket = buckets_[hash & mask];

  auto iter = bucket.begin();
  auto iter_prev = iter;
  while (iter != bucket.end()) {
    if (iter->hash != hash || iter->val != val) {
      iter_prev = iter;
      ++iter;
      continue;
    }
    auto rv = Optional<T>::New(std::move(iter->val));
    if (iter_prev == iter) {
      bucket.RemoveHead();
    } else {
      iter_prev.RemoveAfter();
    }
    --size_;
    return rv;
  }
}

template <HashSetValType T>
typename HashSet<T>::ConstIterator HashSet<T>::TryFind(const T& val) const {
  if (size_ == 0) {
    return end();
  }
  const size_t hash = hasher_(val);
  const size_t mask = buckets_.GetSize() - 1;
  const size_t bucket_index = hash & mask;
  auto bucket_iter = buckets_.begin() + bucket_index;
  for (auto iter = bucket_iter->begin(); iter != bucket_iter->end(); ++iter) {
    if (iter->hash == hash && iter->val == val) {
      return ConstIterator(bucket_iter, buckets_.end(), iter);
    }
  }
  return end();
}

template <HashSetValType T>
void HashSet<T>::Clear() {
  buckets_.Clear();
  size_ = 0;
}

template <HashSetValType T>
bool HashSet<T>::IsEmpty() const {
  return size_ == 0;
}

template <HashSetValType T>
size_t HashSet<T>::GetSize() const {
  return size_;
}

template <HashSetValType T>
float HashSet<T>::GetMaxLoadFactor() const {
  return max_load_factor_;
}

template <HashSetValType T>
void HashSet<T>::SetMaxLoadFactor(float load_factor) {
  max_load_factor_ = load_factor;
  if (const float load_factor = static_cast<float>(size_) / buckets_.GetSize();
      load_factor > max_load_factor_) {
    ExtendAndRehash();
  }
}

template <HashSetValType T>
size_t HashSet<T>::GetBucketSize() const {
  return buckets_.GetSize();
}

template <HashSetValType T>
typename HashSet<T>::ConstIterator HashSet<T>::begin() const {
  if (IsEmpty()) {
    return end();
  }
  ConstIterator rv(buckets_.begin(), buckets_.end(), buckets_.begin()->begin());
  if (!rv) ++rv;
  return rv;
}

template <HashSetValType T>
typename HashSet<T>::ConstIterator HashSet<T>::end() const {
  return ConstIterator(buckets_.end(), buckets_.end(), nullptr);
}

template <HashSetValType T>
void HashSet<T>::ExtendAndRehash() {
  const size_t old_size = buckets_.GetSize();
  MIRAGE_DCHECK(old_size != 0);
  MIRAGE_DCHECK((old_size & (old_size - 1)) == 0);  // size should be 2^n

  if (constexpr size_t overflow_mask = static_cast<size_t>(1)
                                       << (sizeof(size_t) * 8 - 1);
      (old_size & overflow_mask) != 0) {
    return;
  }

  const size_t new_size = old_size * 2;
  buckets_.SetSize(new_size);

  for (size_t i = 0; i < old_size; ++i) {
    auto& bucket = buckets_[i];
    if (bucket.IsEmpty()) continue;

    auto iter = bucket.begin();
    auto iter_prev = iter;
    while (iter != bucket.end()) {
      const size_t mask = new_size;
      if (const bool into_new_bucket = (iter->hash & mask) != 0;
          !into_new_bucket) {
        iter_prev = iter;
        ++iter;
        continue;
      }

      auto& new_bucket = buckets_[old_size + i];
      if (iter_prev == iter) {
        new_bucket.EmplaceHead(bucket.RemoveHead());
        iter = bucket.begin();
        iter_prev = bucket.begin();
      } else {
        ++iter;
        new_bucket.EmplaceHead(iter_prev.RemoveAfter());
      }
    }
  }
}

template <HashSetValType T>
HashSet<T>::ConstIterator::ConstIterator(BucketIter bucket_iter,
                                         BucketIter bucket_end,
                                         EntryIter entry_iter)
    : bucket_iter_(bucket_iter),
      bucket_end_(bucket_end),
      entry_iter_(entry_iter) {}

template <HashSetValType T>
typename HashSet<T>::ConstIterator::iterator_type&
HashSet<T>::ConstIterator::operator=(std::nullptr_t) {
  bucket_iter_ = nullptr;
  bucket_end_ = nullptr;
  entry_iter_ = nullptr;
  return *this;
}

template <HashSetValType T>
typename HashSet<T>::ConstIterator::reference
HashSet<T>::ConstIterator::operator*() const {
  return entry_iter_->val;
}

template <HashSetValType T>
typename HashSet<T>::ConstIterator::pointer
HashSet<T>::ConstIterator::operator->() const {
  return &(entry_iter_->val);
}

template <HashSetValType T>
typename HashSet<T>::ConstIterator::iterator_type&
HashSet<T>::ConstIterator::operator++() {
  if (bucket_iter_ == bucket_end_) return *this;

  if (entry_iter_ != nullptr) {
    ++entry_iter_;
    if (entry_iter_ != bucket_iter_->end()) return *this;
  }
  while (true) {
    ++bucket_iter_;
    if (bucket_iter_ == bucket_end_) break;
    if (bucket_iter_->IsEmpty()) continue;
    entry_iter_ = bucket_iter_->begin();
    return *this;
  }
  entry_iter_ = nullptr;
  return *this;
}

template <HashSetValType T>
typename HashSet<T>::ConstIterator::iterator_type
HashSet<T>::ConstIterator::operator++(int) {
  iterator_type rv = *this;
  ++(*this);
  return rv;
}

template <HashSetValType T>
bool HashSet<T>::ConstIterator::operator==(const iterator_type& other) const {
  return bucket_iter_ == other.bucket_iter_ &&
         bucket_end_ == other.bucket_end_ && entry_iter_ == other.entry_iter_;
}

template <HashSetValType T>
bool HashSet<T>::ConstIterator::operator==(std::nullptr_t) const {
  return !bucket_iter_ && !entry_iter_;
}

template <HashSetValType T>
HashSet<T>::ConstIterator::operator bool() const {
  return !this->operator==(nullptr);
}

}  // namespace mirage::base

#endif  // MIRAGE_BASE_CONTAINER_HASH_SET
