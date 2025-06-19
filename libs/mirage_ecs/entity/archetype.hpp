#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE
#define MIRAGE_ECS_ENTITY_ARCHETYPE

#include <cstdint>

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

  template <typename T>
  using Array = base::Array<T>;

 public:
  using ConstView = ArchetypeDataPage::ConstView;
  using View = ArchetypeDataPage::View;

  constexpr static size_t kInvalidSparseId = -1;

  class Courier;

  Archetype() = delete;
  MIRAGE_ECS Archetype(SharedDescriptor &&descriptor,
                       PagePoolObserver &&page_pool);
  MIRAGE_ECS ~Archetype() = default;

  Archetype(const Archetype &) = delete;
  Archetype &operator=(const Archetype &) = delete;

  MIRAGE_ECS Archetype(Archetype &&other) noexcept = default;
  MIRAGE_ECS Archetype &operator=(Archetype &&other) noexcept = default;

  // Return sparse index, -1 if failed
  MIRAGE_ECS size_t Push(const EntityId &id, ComponentBundle &bundle);
  MIRAGE_ECS size_t Push(View &view);

  MIRAGE_ECS Array<ArchetypeDataPage> TakeMany(
      const SharedDescriptor &dest_desc, const Array<size_t> &sparse_id_array);

  MIRAGE_ECS void Remove(size_t sparse_id);
  MIRAGE_ECS void RemoveMany(const Array<size_t> &sparse_id_array);

  MIRAGE_ECS ConstView operator[](size_t sparse_id) const;
  MIRAGE_ECS View operator[](size_t sparse_id);

  MIRAGE_ECS void Clear();

 private:
  MIRAGE_ECS void EnsureNotFull();

  struct MIRAGE_ECS Route {
    size_t page_id;
    size_t offset;
  };
  [[nodiscard]] Route GetRoute(size_t dense_id) const;

  SharedDescriptor descriptor_;

  PagePoolObserver page_pool_;
  Array<ArchetypeDataPage> page_array_;

  Array<Array<size_t>> sparse_;
  Array<size_t> dense_;
  Array<size_t> hole_;

  size_t size_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE
