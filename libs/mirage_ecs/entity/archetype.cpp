#include "mirage_ecs/entity/archetype.hpp"

#include <cstddef>

#include "mirage_base/buffer/aligned_buffer.hpp"
#include "mirage_base/container/array.hpp"
#include "mirage_base/define/check.hpp"
#include "mirage_base/util/constant.hpp"
#include "mirage_ecs/entity/archetype_data_page.hpp"
#include "mirage_ecs/util/aligned_buffer_pool.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

using SparseId = Archetype::SparseId;
using DenseId = Archetype::DenseId;

void Archetype::EnsureNotFull() {
  if (!available_sparse_buffer_.empty()) {
    // TODO
    return;
  }

  constexpr auto pool_buffer_size = AlignedBufferPool::kBufferSize;
  if (sparse_.empty()) {
    // TODO
    return;
  }

  if (descriptor_->size() > pool_buffer_size) {
    // TODO
    return;
  }

  if (page_array_.size() == 1 &&
      page_array_[0].buffer().size() < pool_buffer_size) {
    const auto old_buffer_size = page_array_[0].buffer().size();
    const auto old_capacity = page_array_[0].capacity();

    auto new_buffer_size = old_buffer_size * 2;
    auto new_capacity = old_capacity * 2;

    if (new_buffer_size > pool_buffer_size ||
        pool_buffer_size - new_buffer_size < descriptor_->size()) {
      new_buffer_size = pool_buffer_size;
      new_capacity = pool_buffer_size / descriptor_->size();
    }

    page_array_[0].Reserve(new_buffer_size);
    if (new_capacity > old_capacity) {
      for (auto i = old_capacity; i < new_capacity; ++i) {
        sparse_.Push(kInvalidDenseId);
        hole_.Push(i);
      }
      return;
    }
  }

  MIRAGE_DCHECK(page_pool_);
  auto page = page_pool_->Allocate(descriptor_->align());
  page.Initialize(descriptor_.Clone());

  const auto page_capacity = page.capacity();
  const auto dense_size = dense_.size();
  for (auto i = 0; i < page_capacity; ++i) {
    sparse_.Push(kInvalidDenseId);
    hole_.Push(dense_size + i);
  }
  page_array_.Emplace(std::move(page));
}

Archetype::SparseBuffer::SparseBuffer(Buffer&& buffer)
    : buffer_(std::move(buffer)) {
  MIRAGE_DCHECK(buffer_.size() >= sizeof(HoleCount) * 2);
  MIRAGE_DCHECK(buffer_.align() >= alignof(DenseId));

  hole_count_ref() = 0;

  auto& capacity = capacity_ref();
  capacity = (buffer_.size() - 2 * sizeof(HoleCount)) /
             (sizeof(Hole) + sizeof(DenseId));

  auto* hole_begin = HoleBegin();
  for (auto i = 0; i < capacity; ++i) {
    hole_begin[i] = capacity - i - 1;
  }
}

DenseId const& Archetype::SparseBuffer::operator[](size_t index) const {
  MIRAGE_DCHECK(index < capacity());
  return DenseIdBegin()[index];
}

DenseId& Archetype::SparseBuffer::operator[](size_t index) {
  MIRAGE_DCHECK(index < capacity());
  return DenseIdBegin()[index];
}

DenseId Archetype::SparseBuffer::TakeDenseId(size_t index) {
  MIRAGE_DCHECK(index < capacity());
  auto& dense_id = DenseIdBegin()[index];
  auto rv = dense_id;
  dense_id = kInvalidDenseId;

  auto& count = hole_count_ref();
  HoleBegin()[count] = static_cast<Hole>(index);
  ++count;

  return rv;
}

Archetype::SparseBuffer::Hole Archetype::SparseBuffer::PopHole() {
  MIRAGE_DCHECK(hole_count() > 0);
  auto& count = hole_count_ref();
  --count;
  return HoleBegin()[count];
}

Archetype::Buffer& Archetype::SparseBuffer::buffer() { return buffer_; }

Archetype::SparseBuffer::HoleCount Archetype::SparseBuffer::hole_count() const {
  return hole_count_ref();
}

Archetype::SparseBuffer::HoleCount Archetype::SparseBuffer::capacity() const {
  return capacity_ref();
}

Archetype::SparseBuffer::HoleCount& Archetype::SparseBuffer::hole_count_ref()
    const {
  return *reinterpret_cast<HoleCount*>(const_cast<std::byte*>(buffer_.ptr()));
}

Archetype::SparseBuffer::HoleCount& Archetype::SparseBuffer::capacity_ref()
    const {
  return *(&hole_count_ref() + 1);
}

Archetype::SparseBuffer::Hole* Archetype::SparseBuffer::HoleBegin() const {
  auto* dense_id_begin = DenseIdBegin();
  return reinterpret_cast<Hole*>(dense_id_begin) - capacity();
}

DenseId* Archetype::SparseBuffer::DenseIdBegin() const {
  auto* buffer_ptr = const_cast<std::byte*>(buffer_.ptr());
  const size_t offset = buffer_.size() - sizeof(DenseId) * capacity();
  return reinterpret_cast<DenseId*>(buffer_ptr + offset);
}

// Archetype::Archetype(SharedDescriptor&& descriptor,
//                      BufferPoolObserver&& buffer_pool)
//     : descriptor_(std::move(descriptor)),
//     buffer_pool_(std::move(buffer_pool)) {
//   MIRAGE_DCHECK(descriptor_);
//   MIRAGE_DCHECK(buffer_pool_);
// }

// SparseId Archetype::Push(const EntityId& id, ComponentBundle& bundle) {
//   EnsureNotFull();
//   // TODO
//   ++size_;
//   return 0;
// }

// SparseId Archetype::Push(View& view) {
//   EnsureNotFull();
//   // TODO
//   ++size_;
//   return 0;
// }

// void Archetype::Remove(SparseId id) {
//   // TODO: Swap remove dense
//   --size_;
// }

// void Archetype::RemoveMany(const Array<SparseId>& id_array) {
//   for (auto id : id_array) {
//     Remove(id);
//   }
// }

// Archetype::ConstView Archetype::operator[](SparseId id) const {
//   auto route = GetRoute(id);
//   return page_array_[route.page_id][route.offset];
// }

// Archetype::View Archetype::operator[](SparseId id) {
//   auto route = GetRoute(id);
//   return page_array_[route.page_id][route.offset];
// }

// void Archetype::Clear() {
//   while (!page_array_.empty()) {
//     auto page = page_array_.Pop();
//     page.Reset();
//     buffer_pool_->Release(std::move(page.buffer()));
//   }
//   sparse_.Clear();
//   dense_.Clear();
//   hole_.Clear();
// }

// size_t Archetype::size() const { return size_; }
