#ifndef MIRAGE_FRAMEWORK_TASK_TASK
#define MIRAGE_FRAMEWORK_TASK_TASK

#include "mirage_framework/define.hpp"

namespace mirage {

class MIRAGE_API Task {
 public:
  enum Status { Running, Done };

  virtual ~Task() = default;
  virtual Status Update() = 0;
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_TASK_TASK
