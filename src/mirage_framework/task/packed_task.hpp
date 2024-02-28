#ifndef MIRAGE_FRAMEWORK_TASK_PACKED_TASK
#define MIRAGE_FRAMEWORK_TASK_PACKED_TASK

#include "mirage_framework/base/auto_ptr/owned.hpp"
#include "mirage_framework/define.hpp"
#include "mirage_framework/task/task.hpp"

namespace mirage {

class PackedTask {
 public:
  MIRAGE_API PackedTask() = default;
  MIRAGE_API PackedTask(PackedTask&& other);
  MIRAGE_API explicit PackedTask(Owned<Task>&& task);

  MIRAGE_API PackedTask& operator=(PackedTask&& other);

  MIRAGE_API void Update();
  MIRAGE_API Task::Status GetStatus();

  PackedTask(const PackedTask&) = delete;

 private:
  Owned<Task> task_{nullptr};
  Task::Status status_{Task::Done};
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_TASK_PACKED_TASK
