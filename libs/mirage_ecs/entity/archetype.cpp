#include "mirage_ecs/entity/archetype.hpp"

#include <algorithm>
#include <utility>

#include "mirage_base/define/check.hpp"
#include "mirage_base/memory/aligned_buffer.hpp"
#include "mirage_base/util/constant.hpp"
#include "mirage_ecs/entity/buffer/archetype_data_buffer.hpp"
#include "mirage_ecs/entity/buffer/sparse_dense_buffer.hpp"
#include "mirage_ecs/entity/generation_id.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

using Index = Archetype::Index;
using ConstView = Archetype::ConstView;
using View = Archetype::View;

constexpr static inline uint16_t kMaxBufferSize = 16 * kKB;

Archetype::Archetype(SharedDescriptor &&descriptor)
    : descriptor_(std::move(descriptor)) {}

Index Archetype::Push(const EntityId &id, ComponentBundle &bundle) {
  EnsureNotFull();
  ++size_;
  auto &data_buffer = data_.Tail();
  data_buffer.Push(id, bundle);
  return PushSparseDenseBuffer();
}

Index Archetype::Push(View &&view) {
  EnsureNotFull();
  ++size_;
  auto &data_buffer = data_.Tail();
  data_buffer.Push(std::move(view));
  return PushSparseDenseBuffer();
}

ConstView Archetype::operator[](Index index) const {
  return const_cast<Archetype &>(*this)[index];
}

View Archetype::operator[](Index index) {
  const auto sparse_route = GetSparseRoute(index);
  const auto dense_id = sparse_[sparse_route.id][sparse_route.offset];
  MIRAGE_DCHECK(dense_id != kInvalidDenseId);
  const auto data_route = GetDataRoute(dense_id);
  return data_[data_route.id][data_route.offset];
}

Array<ArchetypeDataBuffer> Archetype::TakeMany(SharedDescriptor &&target,
                                               Array<Index> &&index_list) {
  if (index_list.empty()) {
    return Array<ArchetypeDataBuffer>();
  }

  for (auto &index : index_list) {
    index = TakeDenseIdFromSparse(index);
  }

  Array<ArchetypeDataBuffer> take_buffer;
  const auto index_list_size = index_list.size();
  const auto unit_size = target->size() + sizeof(EntityId);
  const auto target_align = target->align();
  const auto id_align = alignof(EntityId);
  const auto align = target_align > id_align ? target_align : id_align;
  const auto capacity = kMaxBufferSize / unit_size;
  if (capacity == 0) {
    auto buffer_cnt = index_list_size;
    take_buffer.Reserve(buffer_cnt);
    while (buffer_cnt != 0) {
      --buffer_cnt;
      take_buffer.Emplace(AlignedBuffer{unit_size, align}, target.Clone());
    }
  } else if (capacity < index_list_size) {
    auto buffer_cnt = (index_list_size / capacity) + 1;
    auto index_cnt = index_list_size;
    take_buffer.Reserve(buffer_cnt);
    while (buffer_cnt != 1) {
      --buffer_cnt;
      index_cnt -= capacity;
      take_buffer.Emplace(AlignedBuffer{kMaxBufferSize, align}, target.Clone());
    }
    take_buffer.Emplace(AlignedBuffer{unit_size * index_cnt, align},
                        target.Clone());
  } else {
    take_buffer.Emplace(AlignedBuffer{unit_size * index_list_size, align},
                        target.Clone());
  }

  auto iter = index_list.begin();
  for (auto &buffer : take_buffer) {
    while (!buffer.is_full() && iter != index_list.end()) {
      auto data_route = GetDataRoute(*iter);
      buffer.Push(data_[data_route.id][data_route.offset]);
      ++iter;
    }
  }

  RemoveManyDenseDataBuffer(std::move(index_list));
  return take_buffer;
}

void Archetype::Remove(Index index) {
  RemoveDenseDataBuffer(TakeDenseIdFromSparse(index));
}

void Archetype::RemoveMany(Array<Index> &&index_list) {
  if (index_list.empty()) {
    return;
  }
  for (auto &index : index_list) {
    index = TakeDenseIdFromSparse(index);
  }
  RemoveManyDenseDataBuffer(std::move(index_list));
}

size_t Archetype::size() const { return size_; }

void Archetype::EnsureNotFull() {
  EnsureNotFullSparse();
  EnsureNotFullDense();
  EnsureNotFullData();
}

void Archetype::EnsureNotFullSparse() {
  if (!available_sparse_.empty()) {
    return;
  }

  if (sparse_.empty()) {
    available_sparse_.Emplace(0);
    sparse_.Emplace(
        AlignedBuffer{SparseBuffer::kUnitSize, SparseBuffer::kAlign});
    return;
  }
  if (sparse_.size() == 1) {
    const auto new_byte_size = sparse_[0].buffer().size() * 2;
    if (new_byte_size <= kMaxBufferSize &&
        kMaxBufferSize - new_byte_size >= SparseBuffer::kUnitSize) {
      sparse_[0].Reserve(new_byte_size);
    } else {
      sparse_[0].Reserve(kMaxBufferSize);
    }
    available_sparse_.Emplace(0);
    return;
  }

  available_sparse_.Emplace(sparse_.size());
  sparse_.Emplace(AlignedBuffer{kMaxBufferSize, SparseBuffer::kAlign});
}

void Archetype::EnsureNotFullDense() {
  if (!dense_.empty() && !dense_.Tail().is_full()) {
    return;
  }

  if (dense_.empty()) {
    dense_.Emplace(AlignedBuffer{DenseBuffer::kUnitSize, DenseBuffer::kAlign});
    return;
  } else if (dense_.size() == 1) {
    const auto new_byte_size = dense_[0].buffer().size() * 2;
    if (new_byte_size <= kMaxBufferSize &&
        kMaxBufferSize - new_byte_size >= DenseBuffer::kUnitSize) {
      dense_[0].Reserve(new_byte_size);
    } else {
      dense_[0].Reserve(kMaxBufferSize);
    }

    if (!dense_[0].is_full()) {
      return;
    }
  }
  dense_.Emplace(AlignedBuffer{kMaxBufferSize, DenseBuffer::kAlign});
}

void Archetype::EnsureNotFullData() {
  if (!data_.empty() && !data_.Tail().is_full()) {
    return;
  }

  const auto unit_size = ArchetypeDataBuffer::unit_size(*descriptor_);

  const auto desc_align = descriptor_->align();
  const auto id_align = alignof(EntityId);
  const auto align = desc_align > id_align ? desc_align : id_align;
  static_assert(kMaxBufferSize >= alignof(EntityId));

  if (data_.empty()) {
    data_.Emplace(AlignedBuffer{unit_size, align}, descriptor_.Clone());
    return;
  } else if (data_.size() == 1) {
    const auto new_byte_size = data_[0].buffer().size() * 2;
    if (new_byte_size <= kMaxBufferSize &&
        kMaxBufferSize - new_byte_size >= unit_size) {
      data_[0].Reserve(new_byte_size);
    } else {
      data_[0].Reserve(kMaxBufferSize);
    }

    if (!data_[0].is_full()) {
      return;
    }
  }
  data_.Emplace(AlignedBuffer{kMaxBufferSize, align}, descriptor_.Clone());
}

SparseId Archetype::PushSparseDenseBuffer() {
  MIRAGE_DCHECK(available_sparse_.size() > 0);

  const auto sparse_buffer_id = available_sparse_[0];
  auto &sparse_buffer = sparse_[sparse_buffer_id];
  if (sparse_buffer.capacity() == 0) {
    if (sparse_.size() == 1) {
      sparse_buffer = SparseBuffer(
          AlignedBuffer{SparseBuffer::kUnitSize, SparseBuffer::kAlign});
    } else {
      sparse_buffer =
          SparseBuffer(AlignedBuffer{kMaxBufferSize, SparseBuffer::kAlign});
    }
  }

  if (sparse_buffer.hole_cnt() == 1) {
    available_sparse_.SwapRemove(0);
  }
  MIRAGE_DCHECK(sparse_buffer.hole_cnt() > 0);

  auto &dense_buffer = dense_.Tail();

  DenseId dense_id =
      dense_[0].capacity() * (dense_.size() - 1) + dense_buffer.size();
  SparseId sparse_id = sparse_[0].capacity() * sparse_buffer_id +
                       sparse_buffer.FillHole(dense_id);
  dense_buffer.Push(sparse_id);
  return sparse_id;
}

Archetype::Route Archetype::GetSparseRoute(SparseId sparse_id) {
  const auto capacity = sparse_[0].capacity() != 0
                            ? sparse_[0].capacity()
                            : (kMaxBufferSize / SparseBuffer::kUnitSize);
  const auto id = sparse_id / capacity;
  const auto offset = static_cast<uint16_t>(sparse_id - id * capacity);

  MIRAGE_DCHECK(sparse_.size() > id);
  MIRAGE_DCHECK(sparse_[id].capacity() > offset);
  return {id, offset};
}

Archetype::Route Archetype::GetDenseRoute(DenseId dense_id) {
  const auto capacity = dense_[0].capacity();
  const auto id = dense_id / capacity;
  const auto offset = static_cast<uint16_t>(dense_id - id * capacity);

  MIRAGE_DCHECK(dense_.size() > id);
  MIRAGE_DCHECK(dense_[id].size() > offset);
  return {id, offset};
}

Archetype::Route Archetype::GetDataRoute(DenseId dense_id) {
  const auto capacity = data_[0].capacity();
  const auto id = dense_id / capacity;
  const auto offset = static_cast<uint16_t>(dense_id - id * capacity);

  MIRAGE_DCHECK(data_.size() > id);
  MIRAGE_DCHECK(data_[id].size() > offset);
  return {id, offset};
}

DenseId Archetype::TakeDenseIdFromSparse(SparseId sparse_id) {
  auto route = GetSparseRoute(sparse_id);
  auto &sparse_buffer = sparse_[route.id];
  if (sparse_buffer.hole_cnt() == 0) {
    available_sparse_.Push(route.id);
  }

  auto rv = sparse_buffer.Remove(route.offset);

  if (sparse_buffer.size() == 0) {
    if (sparse_.size() > 1) {
      sparse_buffer = SparseBuffer();
    } else {
      sparse_.Clear();
      available_sparse_.Clear();
    }
  }

  return rv;
}

void Archetype::RemoveDenseDataBuffer(DenseId dense_id) {
  --size_;

  // Remove dense buffer
  auto &dense_tail_buffer = dense_.Tail();
  SparseId &dense_tail = dense_tail_buffer[dense_tail_buffer.size() - 1];
  const auto dense_route = GetDenseRoute(dense_id);
  dense_[dense_route.id][dense_route.offset] = dense_tail;
  dense_tail_buffer.RemoveTail();
  if (dense_tail_buffer.size() == 0) {
    dense_.RemoveTail();
  }

  // Remove data buffer
  auto &data_tail_buffer = data_.Tail();
  auto data_tail = data_tail_buffer[data_tail_buffer.size() - 1];
  const auto data_route = GetDataRoute(dense_id);
  auto data = data_[data_route.id][data_route.offset];

  data.entity_id() = data_tail.entity_id();
  data_tail.entity_id().Reset();

  auto *data_view_ptr = data.view_ptr();
  auto *data_tail_view_ptr = data_tail.view_ptr();
  for (auto &entry : descriptor_->offset_map()) {
    const auto component_id = entry.key();
    const auto offset = entry.val();
    component_id.destruct(data_view_ptr + offset);
    component_id.move(data_tail_view_ptr + offset, data_view_ptr + offset);
  }
  data_tail_buffer.RemoveTail();
  if (data_tail_buffer.size() == 0) {
    data_.RemoveTail();
  }
}

void Archetype::RemoveManyDenseDataBuffer(Array<DenseId> &&dense_list) {
  std::ranges::sort(dense_list, std::greater<DenseId>());
  for (const auto &dense_id : dense_list) {
    RemoveDenseDataBuffer(dense_id);
  }
}
