#ifndef MIRAGE_FRAMEWORK_TASK_ASYNC_QUEUE
#define MIRAGE_FRAMEWORK_TASK_ASYNC_QUEUE

#include <concepts>
#include <mutex>
#include <optional>
#include <queue>

namespace mirage {

template <typename T>
concept AsyncQueueValue = std::default_initializable<T> && std::movable<T>;

template <AsyncQueueValue T>
class AsyncQueue {
 public:
  AsyncQueue() = default;
  ~AsyncQueue() = default;

  AsyncQueue(AsyncQueue&& other) noexcept {
    std::lock_guard<std::mutex> lock(other.mutex_);
    raw_queue_ = std::move(other.raw_queue_);
  }

  AsyncQueue(AsyncQueue& other) {
    if constexpr (!std::copyable<T>) {
      throw std::invalid_argument("This type is supposed to be copyable.");
    } else {
      std::lock_guard<std::mutex> lock(other.mutex_);
      raw_queue_ = other.raw_queue_;
    }
  }

  bool IsEmpty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return raw_queue_.empty();
  }

  size_t GetSize() {
    std::lock_guard<std::mutex> lock(mutex_);
    return raw_queue_.size();
  }

  void Push(const T& val) {
    if constexpr (!std::copyable<T>) {
      throw std::invalid_argument("This type is supposed to be copyable.");
    } else {
      std::lock_guard<std::mutex> lock(mutex_);
      raw_queue_.push(val);
    }
  }

  void Emplace(T&& val) {
    std::lock_guard<std::mutex> lock(mutex_);
    raw_queue_.emplace(std::move(val));
  }

  std::optional<T> Pop() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsEmpty()) {
      return std::nullopt;
    }
    T val(std::move(raw_queue_.front()));
    raw_queue_.pop();
    return std::move(val);
  }

 private:
  std::queue<T> raw_queue_;
  std::mutex mutex_;
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_TASK_ASYNC_QUEUE
