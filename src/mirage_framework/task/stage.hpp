#ifndef MIRAGE_FRAMEWORK_TASK_STAGE
#define MIRAGE_FRAMEWORK_TASK_STAGE

#include "mirage_framework/base/auto_ptr/shared.hpp"
#include "mirage_framework/base/auto_ptr/weak.hpp"
#include "mirage_framework/base/container/array.hpp"
#include "mirage_framework/define.hpp"

namespace mirage {
class Stage;
}

INSTANTIATE_ARRAY(mirage::WeakAsync<mirage::Stage>);
INSTANTIATE_ARRAY(mirage::SharedAsync<mirage::Stage>);

namespace mirage {

class MIRAGE_API Stage {
 public:
  Stage() = default;
  Stage(const Stage&) = delete;
  Stage(Stage&& other);
  ~Stage() = default;

 private:
  Array<WeakAsync<Stage>> prev_;
  Array<SharedAsync<Stage>> next_;
};

}  // namespace mirage

#endif  // MIRAGE_FRAMEWORK_TASK_STAGE