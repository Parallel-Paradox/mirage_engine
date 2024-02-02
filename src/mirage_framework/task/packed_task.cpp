#include "mirage_framework/task/packed_task.hpp"

using namespace mirage;

PackedTask::PackedTask(PackedTask&& other)
    : task_(std::move(other.task_)), status_(other.status_) {}

PackedTask::PackedTask(Owned<Task>&& task)
    : task_(std::move(task)), status_(Task::Running) {}

PackedTask& PackedTask::operator=(PackedTask&& other) {
  if (this != &other) {
    this->~PackedTask();
    new (this) PackedTask(std::move(other));
  }
  return *this;
}

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
