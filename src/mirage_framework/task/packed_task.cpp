#include "mirage_framework/task/packed_task.hpp"

using namespace mirage;

PackedTask::PackedTask(Owned<Task>&& task)
    : task_(std::move(task)), status_(Task::Running) {}

void PackedTask::Update() {
  MIRAGE_DCHECK(!task_.IsNull());
  if (status_ == Task::Done) {
    return;
  }
  status_ = task_->Update();
}

Task::Status PackedTask::GetStatus() {
  return status_;
}
