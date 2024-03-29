#ifndef MIRAGE_FRAMEWORK_TASK_STAGE
#define MIRAGE_FRAMEWORK_TASK_STAGE

#include "mirage_framework/base/auto_ptr/shared.hpp"
#include "mirage_framework/base/auto_ptr/weak.hpp"
#include "mirage_framework/base/container/array.hpp"
#include "mirage_framework/define.hpp"

namespace mirage {

class Stage {
 public:
  MIRAGE_API Stage() = default;
  MIRAGE_API Stage(Stage&& other);
  MIRAGE_API ~Stage() = default;

  Stage(const Stage&) = delete;

 private:
  Array<WeakAsync<Stage>> prev_;
  Array<SharedAsync<Stage>> next_;
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_TASK_STAGE
