#ifndef MIRAGE_FRAMEWORK_TASK_PACKED_TASK
#define MIRAGE_FRAMEWORK_TASK_PACKED_TASK

#include "mirage_framework/base/auto_ptr/owned.hpp"
#include "mirage_framework/define.hpp"
#include "mirage_framework/task/task.hpp"

INSTANTIATE_OWNED(mirage::Task)

namespace mirage {

class MIRAGE_API PackedTask {
 public:
  PackedTask() = default;
  explicit PackedTask(Owned<Task>&& task);

  void Update();
  Task::Status GetStatus();

 private:
  Owned<Task> task_{nullptr};
  Task::Status status_{Task::Done};
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_TASK_PACKED_TASK
