#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE
#define MIRAGE_ECS_ENTITY_ARCHETYPE

#include "mirage_base/auto_ptr/observed.hpp"
#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/container/array.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"
#include "mirage_ecs/entity/buffer/aligned_buffer_pool.hpp"
#include "mirage_ecs/entity/buffer/archetype_data_buffer.hpp"
#include "mirage_ecs/entity/buffer/sparse_dense_buffer.hpp"
#include "mirage_ecs/entity/entity_id.hpp"

namespace mirage::ecs {

class Archetype {
  using SharedDescriptor = base::SharedLocal<ArchetypeDescriptor>;
  using BufferPoolObserver = base::LocalObserver<AlignedBufferPool>;

  template <typename T>
  using Array = base::Array<T>;

 public:
  using ConstView = ArchetypeDataBuffer::ConstView;
  using View = ArchetypeDataBuffer::View;

  using Index = SparseId;

  Archetype() = delete;
  MIRAGE_ECS Archetype(const SharedDescriptor &descriptor,
                       BufferPoolObserver &&buffer_pool);
  MIRAGE_ECS ~Archetype() = default;

  Archetype(const Archetype &) = delete;
  Archetype &operator=(const Archetype &) = delete;

  MIRAGE_ECS Archetype(Archetype &&other) noexcept = default;
  MIRAGE_ECS Archetype &operator=(Archetype &&other) noexcept = default;

  MIRAGE_ECS Index Push(const EntityId &id, ComponentBundle &bundle);
  MIRAGE_ECS Index Push(const EntityId &id, View &view);

  MIRAGE_ECS ConstView operator[](Index index) const;
  MIRAGE_ECS View operator[](Index index);

  MIRAGE_ECS Array<ArchetypeDataBuffer> TakeMany(
      const SharedDescriptor &target, const Array<Index> &index_list);

  MIRAGE_ECS void Remove(Index index);
  MIRAGE_ECS void RemoveMany(const Array<Index> &index_list);

 private:
  SharedDescriptor descriptor_;

  BufferPoolObserver buffer_pool_;
  Array<SparseBuffer> sparse_;
  Array<DenseBuffer> dense_;
  Array<ArchetypeDataBuffer> data_;
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE
