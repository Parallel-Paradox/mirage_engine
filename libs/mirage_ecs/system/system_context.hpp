#ifndef MIRAGE_ECS_SYSTEM_SYSTEM_CONTEXT
#define MIRAGE_ECS_SYSTEM_SYSTEM_CONTEXT

#include "mirage_base/container/array.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/generation_id.hpp"

namespace mirage::ecs {

class SystemContext {
  template <typename T>
  using Array = base::Array<T>;

 public:
  [[nodiscard]] MIRAGE_ECS bool ConflictWith(const SystemContext &other) const;

 private:
  Array<ArchetypeId> interested_archetype_array_;
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_SYSTEM_SYSTEM_CONTEXT
