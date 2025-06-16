#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE
#define MIRAGE_ECS_ENTITY_ARCHETYPE

#include "mirage_base/auto_ptr/observed.hpp"
#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/container/array.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/archetype_data_page.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"
#include "mirage_ecs/entity/archetype_page_pool.hpp"
#include "mirage_ecs/entity/entity_id.hpp"

namespace mirage::ecs {

class Archetype {
  using SharedDescriptor = base::SharedLocal<ArchetypeDescriptor>;
  using PagePoolObserver = base::LocalObserver<ArchetypePagePool>;
  using Courier = ArchetypeDataPage::Courier;

  template <typename T>
  using Array = base::Array<T>;

 public:
  Archetype() = delete;
  MIRAGE_ECS Archetype(ArchetypeDescriptor &&descriptor);
  MIRAGE_ECS ~Archetype() = default;

  Archetype(const Archetype &) = delete;
  Archetype &operator=(const Archetype &) = delete;

  MIRAGE_ECS Archetype(Archetype &&other) noexcept = default;
  MIRAGE_ECS Archetype &operator=(Archetype &&other) noexcept = default;

  MIRAGE_ECS void Push(EntityId id, ComponentBundle &bundle);
  MIRAGE_ECS void Push(EntityId id, Courier &&courier);

  struct MIRAGE_ECS ConstView {
    EntityId entity_id;
    ArchetypeDataPage::ConstView view;

    ArchetypeDataPage::ConstView *operator->() { return &view; }
  };
  MIRAGE_ECS ConstView operator[](size_t index) const;

  struct MIRAGE_ECS View {
    EntityId entity_id;
    ArchetypeDataPage::View view;

    ArchetypeDataPage::View *operator->() { return &view; }
  };
  MIRAGE_ECS View operator[](size_t index);

  MIRAGE_ECS Courier PopMany(std::initializer_list<size_t> index_list);
  MIRAGE_ECS void RemoveMany(std::initializer_list<size_t> index_list);

 private:
  SharedDescriptor descriptor_;

  PagePoolObserver page_pool_;
  Array<ArchetypeDataPage> page_array_;
  Array<EntityId> entity_id_array_;

  Array<size_t> sparse_index_array_;
  Array<size_t> hole_array_;
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE
