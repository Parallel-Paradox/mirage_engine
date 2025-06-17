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
  using ConstView = ArchetypeDataPage::ConstView;
  using View = ArchetypeDataPage::View;
  using Courier = ArchetypeDataPage::Courier;

  template <typename T>
  using Array = base::Array<T>;

 public:
  Archetype() = delete;
  MIRAGE_ECS Archetype(const SharedDescriptor &descriptor);
  MIRAGE_ECS ~Archetype() = default;

  Archetype(const Archetype &) = delete;
  Archetype &operator=(const Archetype &) = delete;

  MIRAGE_ECS Archetype(Archetype &&other) noexcept = default;
  MIRAGE_ECS Archetype &operator=(Archetype &&other) noexcept = default;

  MIRAGE_ECS void Push(EntityId id, ComponentBundle &bundle);
  MIRAGE_ECS void Push(Courier &&courier);

  MIRAGE_ECS ConstView operator[](size_t index) const;
  MIRAGE_ECS View operator[](size_t index);

  MIRAGE_ECS Courier PopMany(const Array<size_t> &index_list);
  MIRAGE_ECS void RemoveMany(const Array<size_t> &index_list);

 private:
  SharedDescriptor descriptor_;

  Array<size_t> sparse_;
  Array<size_t> dense_;
  Array<size_t> hole_;

  PagePoolObserver page_pool_;
  Array<ArchetypeDataPage> page_array_;
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE
