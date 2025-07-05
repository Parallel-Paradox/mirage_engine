#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE
#define MIRAGE_ECS_ENTITY_ARCHETYPE

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/container/array.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"
#include "mirage_ecs/entity/buffer/archetype_data_buffer.hpp"
#include "mirage_ecs/entity/buffer/sparse_dense_buffer.hpp"
#include "mirage_ecs/entity/entity_id.hpp"

namespace mirage::ecs {

class Archetype {
  using SharedDescriptor = base::SharedLocal<ArchetypeDescriptor>;

  template <typename T>
  using Array = base::Array<T>;

 public:
  using ConstView = ArchetypeDataBuffer::ConstView;
  using View = ArchetypeDataBuffer::View;

  using Index = SparseId;

  Archetype() = default;
  MIRAGE_ECS Archetype(SharedDescriptor &&descriptor);
  MIRAGE_ECS ~Archetype() = default;

  Archetype(const Archetype &) = delete;
  Archetype &operator=(const Archetype &) = delete;

  MIRAGE_ECS Archetype(Archetype &&other) noexcept = default;
  MIRAGE_ECS Archetype &operator=(Archetype &&other) noexcept = default;

  MIRAGE_ECS Index Push(const EntityId &id, ComponentBundle &bundle,
                        AlignedBufferPool &buffer_pool);
  MIRAGE_ECS Index Push(View &&view, AlignedBufferPool &buffer_pool);

  MIRAGE_ECS ConstView operator[](Index index) const;
  MIRAGE_ECS View operator[](Index index);

  MIRAGE_ECS Array<ArchetypeDataBuffer> TakeMany(
      SharedDescriptor &&target, Array<Index> &&index_list,
      AlignedBufferPool &buffer_pool);

  MIRAGE_ECS void Remove(Index index, AlignedBufferPool &buffer_pool);
  MIRAGE_ECS void RemoveMany(Array<Index> &&index_list,
                             AlignedBufferPool &buffer_pool);

 private:
  MIRAGE_ECS void EnsureNotFull(AlignedBufferPool &buffer_pool);
  MIRAGE_ECS SparseId PushSparseDenseBuffer(AlignedBufferPool &buffer_pool);

  struct MIRAGE_ECS Route {
    size_t id{0};
    uint16_t offset{0};
  };
  MIRAGE_ECS Route GetSparseRoute(SparseId sparse_id);
  MIRAGE_ECS Route GetDenseRoute(DenseId dense_id);
  MIRAGE_ECS Route GetDataRoute(DenseId dense_id);

  MIRAGE_ECS DenseId TakeDenseIdFromSparse(SparseId sparse_id,
                                           AlignedBufferPool &buffer_pool);

  MIRAGE_ECS void RemoveDenseDataBuffer(DenseId dense_id,
                                        AlignedBufferPool &buffer_pool);
  MIRAGE_ECS void RemoveManyDenseDataBuffer(Array<DenseId> &&dense_list,
                                            AlignedBufferPool &buffer_pool);

  SharedDescriptor descriptor_;

  Array<SparseBuffer> sparse_;
  Array<size_t> available_sparse_;
  Array<DenseBuffer> dense_;

  Array<ArchetypeDataBuffer> data_;

  size_t size_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE
