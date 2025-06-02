#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE
#define MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"

namespace mirage::ecs {

class ArchetypeDataPage {
  using SharedLocalDescriptor = base::SharedLocal<ArchetypeDescriptor>;

 public:
  ArchetypeDataPage() = delete;
  MIRAGE_ECS ~ArchetypeDataPage();

  ArchetypeDataPage(const ArchetypeDataPage&) = delete;
  ArchetypeDataPage& operator=(const ArchetypeDataPage&) = delete;

  MIRAGE_ECS ArchetypeDataPage(ArchetypeDataPage&&);
  MIRAGE_ECS ArchetypeDataPage& operator=(ArchetypeDataPage&&);

  MIRAGE_ECS ArchetypeDataPage(size_t byte_size);

  MIRAGE_ECS void Initialize(SharedLocalDescriptor descriptor);
  MIRAGE_ECS void Reset();

  MIRAGE_ECS void Clear();

 private:
  SharedLocalDescriptor descriptor_{nullptr};
  size_t capacity_{0};
  size_t size_{0};

  std::byte* buffer_ptr_{nullptr};
  size_t align_padding_{0};
  size_t byte_size_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE
