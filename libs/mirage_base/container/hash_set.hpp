#ifndef MIRAGE_BASE_CONTAINER_HASH_SET
#define MIRAGE_BASE_CONTAINER_HASH_SET

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

  Optional<T> Insert(T&& val);
  Optional<T> Remove(const T& val);
  Optional<T> Remove(const ConstIterator& target);

  ConstIterator TryFind(const T& val) const;

  void Clear();
  [[nodiscard]] bool IsEmpty() const;
  [[nodiscard]] size_t GetSize() const;

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
Optional<T> HashSet<T>::Insert(T&& val) {
  // TODO
  Optional<T> rv = Optional<T>::None();
  ConstIterator iter = TryFind(val);
  if (iter != end()) {
  }

  if (buckets_.IsEmpty()) {
    buckets_.SetSize(16);
  }

  if (const float load_factor = static_cast<float>(size_) / buckets_.GetSize();
      load_factor > max_load_factor_) {
    ExtendAndRehash();
  }
  return Optional<T>::None();
}

template <HashSetValType T>
bool HashSet<T>::IsEmpty() const {
  return size_ == 0;
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
  MIRAGE_DCHECK(!buckets_.IsEmpty());

  const size_t old_size = buckets_.GetSize();
  if (constexpr size_t overflow_mask = static_cast<size_t>(1) << 63;
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
      // const size_t new_bucket_index = old_size + i;
      // TODO: move

      if (iter_prev == iter) {
      }
    }
  }
  // TODO
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
