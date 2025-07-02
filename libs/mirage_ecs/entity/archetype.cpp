#include "mirage_ecs/entity/archetype.hpp"

#include "mirage_base/define/check.hpp"
#include "mirage_ecs/entity/buffer/aligned_buffer_pool.hpp"
#include "mirage_ecs/entity/buffer/sparse_dense_buffer.hpp"
#include "mirage_ecs/entity/entity_id.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

using Index = Archetype::Index;
using ConstView = Archetype::ConstView;
using View = Archetype::View;

Archetype::Archetype(SharedDescriptor &&descriptor,
                     BufferPoolObserver &&buffer_pool)
    : descriptor_(std::move(descriptor)),
      buffer_pool_(std::move(buffer_pool)) {}

Index Archetype::Push(const EntityId &id, ComponentBundle &bundle) {
  EnsureNotFull();
  ++size_;
  auto &data_buffer = data_.Tail();
  data_buffer.Push(id, bundle);
  return PushSparseDenseBuffer();
}

Index Archetype::Push(const EntityId &id, View &&view) {
  EnsureNotFull();
  ++size_;
  auto &data_buffer = data_.Tail();
  data_buffer.Push(id, std::move(view));
  return PushSparseDenseBuffer();
}

ConstView Archetype::operator[](Index index) const {
  return const_cast<Archetype &>(*this)[index];
}

View Archetype::operator[](Index index) {
  MIRAGE_DCHECK(sparse_.size() > 0);
  const auto sparse_buffer_capacity = sparse_[0].capacity();
  const auto sparse_buffer_id = index / sparse_buffer_capacity;

  MIRAGE_DCHECK(sparse_.size() > sparse_buffer_id);
  const auto dense_id =
      sparse_[sparse_buffer_id]
             [index - sparse_buffer_id * sparse_buffer_capacity];

  MIRAGE_DCHECK(data_.size() > 0);
  const auto data_buffer_capacity = data_[0].capacity();
  const auto data_buffer_id = dense_id / data_[0].capacity();

  MIRAGE_DCHECK(data_.size() > data_buffer_id);
  return data_[data_buffer_id]
              [dense_id - data_buffer_id * data_buffer_capacity];
}

Array<ArchetypeDataBuffer> Archetype::TakeMany(const SharedDescriptor &target,
                                               const Array<Index> &index_list) {
  // TODO
  return Array<ArchetypeDataBuffer>();
}

void Archetype::Remove(Index index) {
  // TODO
}

void Archetype::RemoveMany(const Array<Index> &index_list) {
  // TODO
}

void Archetype::EnsureNotFull() {
  if (!available_sparse_.empty() && !dense_.empty() && !data_.empty() &&
      !dense_.Tail().is_full() && !data_.Tail().is_full()) {
    return;
  }

  if (sparse_.empty()) {
    // TODO
  } else if (sparse_.size() == 1) {
    // TODO
  } else {
    available_sparse_.Emplace(sparse_.size());
    sparse_.Emplace(buffer_pool_->Allocate(alignof(DenseId)));
  }

  if (dense_.empty()) {
    // TODO
  } else if (dense_.size() == 1) {
    // TODO
  } else {
    dense_.Emplace(buffer_pool_->Allocate(alignof(SparseId)));
  }

  if (data_.empty()) {
    // TODO
  } else if (data_.size() == 1) {
    // TODO
  } else if (AlignedBufferPool::kBufferSize <
             descriptor_->size() + sizeof(EntityId)) {
    // TODO
    MIRAGE_DCHECK(data_.Tail().capacity() > 0);
  } else {
    const auto desc_align = descriptor_->align();
    const auto id_align = alignof(EntityId);
    const auto align = desc_align > id_align ? desc_align : id_align;
    data_.Emplace(buffer_pool_->Allocate(align));
  }
}

SparseId Archetype::PushSparseDenseBuffer() {
  MIRAGE_DCHECK(available_sparse_.size() > 0);

  const auto sparse_buffer_id = available_sparse_[0];
  auto &sparse_buffer = sparse_[sparse_buffer_id];

  MIRAGE_DCHECK(sparse_buffer.hole_cnt() > 0);
  if (sparse_buffer.hole_cnt() == 1) {
    available_sparse_.SwapRemove(0);
  }

  auto &dense_buffer = dense_.Tail();

  DenseId dense_id =
      dense_[0].capacity() * (dense_.size() - 1) + dense_buffer.size();
  SparseId sparse_id = sparse_[0].capacity() * sparse_buffer_id +
                       sparse_buffer.FillHole(dense_id);
  dense_buffer.Push(sparse_id);
  return sparse_id;
}
