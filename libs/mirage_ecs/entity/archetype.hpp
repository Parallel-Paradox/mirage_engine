#ifndef MIRAGE_ECS_ENTITY_ARCHETYPE
#define MIRAGE_ECS_ENTITY_ARCHETYPE

#include <cstdint>

#include "mirage_base/auto_ptr/observed.hpp"
#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/buffer/aligned_buffer.hpp"
#include "mirage_base/container/array.hpp"
#include "mirage_base/define/check.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/entity/archetype_data_page.hpp"
#include "mirage_ecs/entity/archetype_descriptor.hpp"
#include "mirage_ecs/entity/entity_id.hpp"
#include "mirage_ecs/util/aligned_buffer_pool.hpp"

namespace mirage::ecs {

class Archetype {
  using SharedDescriptor = base::SharedLocal<ArchetypeDescriptor>;
  using BufferPoolObserver = base::LocalObserver<AlignedBufferPool>;
  using Buffer = base::AlignedBuffer;

  template <typename T>
  using Array = base::Array<T>;

 public:
  using ConstView = ArchetypeDataPage::ConstView;
  using View = ArchetypeDataPage::View;

  using SparseId = size_t;
  using DenseId = size_t;
  constexpr static DenseId kInvalidDenseId = SIZE_MAX;

  Archetype() = delete;
  MIRAGE_ECS Archetype(SharedDescriptor &&descriptor,
                       BufferPoolObserver &&buffer_pool);
  MIRAGE_ECS ~Archetype() = default;

  Archetype(const Archetype &) = delete;
  Archetype &operator=(const Archetype &) = delete;

  MIRAGE_ECS Archetype(Archetype &&other) noexcept = default;
  MIRAGE_ECS Archetype &operator=(Archetype &&other) noexcept = default;

  MIRAGE_ECS SparseId Push(const EntityId &id, ComponentBundle &bundle);
  MIRAGE_ECS SparseId Push(View &view);

  MIRAGE_ECS Array<ArchetypeDataPage> TakeMany(
      const SharedDescriptor &dest_desc, const Array<SparseId> &id_array);

  MIRAGE_ECS void Remove(SparseId id);
  MIRAGE_ECS void RemoveMany(const Array<SparseId> &id_array);

  MIRAGE_ECS ConstView operator[](SparseId id) const;
  MIRAGE_ECS View operator[](SparseId id);

  MIRAGE_ECS void Clear();

  MIRAGE_ECS size_t size() const;

 private:
  MIRAGE_ECS void EnsureNotFull();

  SharedDescriptor descriptor_;

  BufferPoolObserver buffer_pool_;
  Array<ArchetypeDataPage> page_array_;

  class MIRAGE_ECS SparseBuffer {
   public:
    using HoleCount = uint16_t;
    using Hole = uint16_t;

    SparseBuffer() = delete;
    SparseBuffer(Buffer &&buffer);
    ~SparseBuffer() = default;

    SparseBuffer(const SparseBuffer &) = delete;
    SparseBuffer &operator=(const SparseBuffer &) = delete;

    SparseBuffer(SparseBuffer &&other) noexcept = default;
    SparseBuffer &operator=(SparseBuffer &&other) noexcept = default;

    DenseId const &operator[](size_t index) const;
    DenseId &operator[](size_t index);

    DenseId TakeDenseId(size_t index);
    Hole PopHole();

    Buffer &buffer();
    [[nodiscard]] HoleCount hole_count() const;
    [[nodiscard]] HoleCount capacity() const;

   private:
    [[nodiscard]] HoleCount &hole_count_ref() const;
    [[nodiscard]] HoleCount &capacity_ref() const;

    [[nodiscard]] Hole *HoleBegin() const;
    [[nodiscard]] DenseId *DenseIdBegin() const;

    // hole_count(HoleCount) | capacity(HoleCount) |
    // (Hole)... | padding | (DenseId)...
    mutable Buffer buffer_;
  };

  Array<SparseBuffer> sparse_;
  Array<size_t> available_sparse_buffer_;

  Array<Buffer> dense_;

  size_t size_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_ARCHETYPE
