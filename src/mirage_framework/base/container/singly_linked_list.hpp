#ifndef MIRAGE_FRAMEWORK_BASE_CONTAINER_SINGLY_LINKED_LIST
#define MIRAGE_FRAMEWORK_BASE_CONTAINER_SINGLY_LINKED_LIST

#include <concepts>
#include <initializer_list>
#include <iterator>

#include "mirage_framework/base/container/concept.hpp"
#include "mirage_framework/define.hpp"

namespace mirage {

namespace {

template <BasicValueType T>
struct Node {
  T val_;
  Node* next_{nullptr};

  Node() = default;
  ~Node() = default;

  Node(T&& val) : val_(std::move(val)) {}

  Node(const T& val) {
    if constexpr (!std::copy_constructible<T>) {
      MIRAGE_DCHECK(false);  // This type is supposed to be copyable.
    } else {
      (&val_)->~T();
      new (&val_) T(val);
    }
  }
};

template <BasicValueType T>
class ConstIterator;

template <BasicValueType T>
class Iterator {
 public:
  using iterator_concept = std::forward_iterator_tag;
  using iterator_category = std::forward_iterator_tag;
  using iterator_type = Iterator;
  using difference_type = int64_t;
  using value_type = T;
  using pointer = value_type*;
  using reference = value_type&;

  using Node = Node<T>;

  Iterator() = default;
  ~Iterator() = default;

  Iterator(const Iterator& other) : here_(other.here_) {}

  explicit Iterator(Node* here) : here_(here) {}

  reference operator*() const { return here_->val_; }

  pointer operator->() const { return &(here_->val); }

  iterator_type& operator++() {
    if (here_ != nullptr) {
      here_ = here_->next_;
    }
    return *this;
  }

  iterator_type operator++(int) {
    if (here_ != nullptr) {
      iterator_type temp = *this;
      here_ = here_->next_;
      return temp;
    } else {
      return *this;
    }
  }

  bool operator==(const iterator_type& other) const {
    return here_ == other.here_;
  }

  template <typename... Args>
  void EmplaceAfter(Args&&... args) {
    Node* new_node = new Node(T(std::forward<Args>(args)...));
    new_node->next_ = here_->next_;
    here_->next_ = new_node;
  }

  void InsertAfter(const T& val) {
    if constexpr (!std::copy_constructible<T>) {
      MIRAGE_DCHECK(false);  // This type is supposed to be copyable.
    } else {
      EmplaceAfter(T(val));
    }
  }

  T RemoveAfter() {
    MIRAGE_DCHECK(here_ != nullptr && here_->next_ != nullptr);
    T val(std::move(here_->next_->val_));
    Node* next = here_->next_;
    here_->next_ = nullptr;
    delete next;
    return std::move(val);
  }

 private:
  friend class ConstIterator<T>;

  Node* here_{nullptr};
};

template <BasicValueType T>
class ConstIterator {
 public:
  using iterator_concept = std::forward_iterator_tag;
  using iterator_category = std::forward_iterator_tag;
  using iterator_type = ConstIterator;
  using difference_type = int64_t;
  using value_type = const T;
  using pointer = value_type*;
  using reference = value_type&;

  using Node = Node<T>;

  ConstIterator() = default;
  ~ConstIterator() = default;

  ConstIterator(const ConstIterator& other) : here_(other.here_) {}

  explicit ConstIterator(Node* here) : here_(here) {}

  explicit ConstIterator(const Iterator<T>& iter) : here_(iter.here_) {}

  reference operator*() const { return here_->val_; }

  pointer operator->() const { return &(here_->val); }

  iterator_type& operator++() {
    if (here_ != nullptr) {
      here_ = here_->next_;
    }
    return *this;
  }

  iterator_type operator++(int) {
    if (here_ != nullptr) {
      iterator_type temp = *this;
      here_ = here_->next_;
      return temp;
    } else {
      return *this;
    }
  }

  bool operator==(const iterator_type& other) const {
    return here_ == other.here_;
  }

 private:
  Node* here_{nullptr};
};

}  // namespace

template <BasicValueType T>
class SinglyLinkedList {
 public:
  using Node = Node<T>;
  using Iterator = Iterator<T>;
  using ConstIterator = ConstIterator<T>;

  SinglyLinkedList() = default;

  SinglyLinkedList(SinglyLinkedList&& other) : head_(other.head_) {
    other.head_ = nullptr;
  }

  SinglyLinkedList(const SinglyLinkedList& other) {
    if constexpr (!std::copy_constructible<T>) {
      MIRAGE_DCHECK(false);  // This type is supposed to be copyable.
    } else {
      auto iter = other.begin();
      if (iter == other.end()) {
        return;
      }
      Node* ptr = new Node(*iter);
      head_ = ptr;
      ++iter;
      while (iter != other.end()) {
        Node* next = new Node(*iter);
        ptr->next_ = next;
        ptr = next;
        ++iter;
      }
    }
  }

  SinglyLinkedList(std::initializer_list<T> list) {
    if constexpr (!std::copy_constructible<T>) {
      MIRAGE_DCHECK(false);  // This type is supposed to be copyable.
    } else {
      if (list.size() == 0) {
        return;
      }
      auto iter = list.begin();
      Node* ptr = new Node(*iter);
      head_ = ptr;
      ++iter;
      while (iter != list.end()) {
        Node* next = new Node(*iter);
        ptr->next_ = next;
        ptr = next;
        ++iter;
      }
    }
  }

  ~SinglyLinkedList() {
    Node* ptr = head_;
    while (ptr != nullptr) {
      Node* next = ptr->next_;
      delete ptr;
      ptr = next;
    }
  }

  template <typename... Args>
  void EmplaceHead(Args&&... args) {
    Node* new_head = new Node(T(std::forward<Args>(args)...));
    new_head->next_ = head_;
    head_ = new_head;
  }

  void PushHead(const T& val) {
    if constexpr (!std::copy_constructible<T>) {
      MIRAGE_DCHECK(false);  // This type is supposed to be copyable.
    } else {
      EmplaceHead(T(val));
    }
  }

  T RemoveHead() {
    MIRAGE_DCHECK(head_ != nullptr);
    T val(std::move(head_->val_));
    Node* head = head_;
    head_ = head_->next_;
    delete head;
    return std::move(val);
  }

  Iterator begin() { return Iterator(head_); }

  Iterator end() { return Iterator(); }

  ConstIterator begin() const { return ConstIterator(head_); }

  ConstIterator end() const { return ConstIterator(); }

 private:
  Node* head_{nullptr};
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_BASE_CONTAINER_SINGLY_LINKED_LIST
