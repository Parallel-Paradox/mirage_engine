#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE
#define MIRAGE_ECS_ENTITY_ARCHETYPE_DATA_PAGE

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_ecs/component/component_bundle.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"

namespace mirage::ecs {

class ArchetypeDataPage {
  using SharedLocalDescriptor = base::SharedLocal<ArchetypeDescriptor>;

 public:
  class View;
  class Slice;

  ArchetypeDataPage() = delete;
  MIRAGE_ECS ~ArchetypeDataPage();

  ArchetypeDataPage(const ArchetypeDataPage&) = delete;
  ArchetypeDataPage& operator=(const ArchetypeDataPage&) = delete;

  MIRAGE_ECS ArchetypeDataPage(ArchetypeDataPage&&);
  MIRAGE_ECS ArchetypeDataPage& operator=(ArchetypeDataPage&&);

  MIRAGE_ECS ArchetypeDataPage(size_t byte_size);

  MIRAGE_ECS void Initialize(SharedLocalDescriptor descriptor);
  MIRAGE_ECS void Reset();

  [[nodiscard]] MIRAGE_ECS bool Push(ComponentBundle& bundle);
  [[nodiscard]] MIRAGE_ECS bool Push(Slice&& slice);
  MIRAGE_ECS Slice SwapRemove(size_t index);

  MIRAGE_ECS void Clear();

  MIRAGE_ECS View operator[](size_t index);
  MIRAGE_ECS const View operator[](size_t index) const;

  [[nodiscard]] MIRAGE_ECS const SharedLocalDescriptor& descriptor() const;
  [[nodiscard]] MIRAGE_ECS size_t capacity() const;
  [[nodiscard]] MIRAGE_ECS size_t size() const;

  [[nodiscard]] MIRAGE_ECS std::byte* buffer_ptr() const;
  [[nodiscard]] MIRAGE_ECS size_t align_padding() const;
  [[nodiscard]] MIRAGE_ECS size_t byte_size() const;

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
