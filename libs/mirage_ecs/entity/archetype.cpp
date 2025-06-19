#include "mirage_ecs/entity/archetype.hpp"

#include "mirage_base/container/array.hpp"
#include "mirage_base/define/check.hpp"
#include "mirage_base/util/constant.hpp"
#include "mirage_ecs/entity/archetype_data_page.hpp"
#include "mirage_ecs/entity/archetype_page_pool.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

Archetype::Archetype(SharedDescriptor&& descriptor, PagePoolObserver&& page)
    : descriptor_(std::move(descriptor)), page_pool_(std::move(page)) {
  MIRAGE_DCHECK(descriptor_);
  MIRAGE_DCHECK(page_pool_);
}

size_t Archetype::Push(const EntityId& id, ComponentBundle& bundle) {
  EnsureNotFull();
  auto page_id = available_page_id_.Tail();
  auto& page = page_array_[page_id];

  auto rv = page.Push(id, bundle);
  MIRAGE_DCHECK(rv >= 0);
  rv += page_id * page.capacity();

  if (page.size() == page.capacity()) {
    available_page_id_.RemoveTail();
  }
  return rv;
}

size_t Archetype::Push(View& view) {
  EnsureNotFull();
  auto page_id = available_page_id_.Tail();
  auto& page = page_array_[page_id];

  auto rv = page.Push(view);
  MIRAGE_DCHECK(rv >= 0);
  rv += page_id * page.capacity();

  if (page.size() == page.capacity()) {
    available_page_id_.RemoveTail();
  }
  return rv;
}

void Archetype::Remove(size_t index) {
  auto route = GetRoute(index);
  page_array_[route.page_id].Remove(route.offset);
}

void Archetype::RemoveMany(const Array<size_t>& index_array) {
  for (auto index : index_array) {
    Remove(index);
  }
}

Archetype::ConstView Archetype::operator[](size_t index) const {
  auto route = GetRoute(index);
  return page_array_[route.page_id][route.offset];
}

Archetype::View Archetype::operator[](size_t index) {
  auto route = GetRoute(index);
  return page_array_[route.page_id][route.offset];
}

void Clear() {
  // TODO
}

void Archetype::EnsureNotFull() {
  if (!hole_.empty()) {
    return;
  }

  constexpr auto max_page_size = ArchetypePagePool::kPageSizeKB * kKB;
  if (page_array_.empty() || descriptor_->size() > max_page_size) {
    const auto index = page_array_.size();
    page_array_.Emplace(descriptor_->size(), descriptor_->align());
    page_array_[index].Initialize(descriptor_.Clone());
    hole_.Push(index);
    return;
  }

  if (page_array_.size() == 1 &&
      page_array_[0].buffer().size() < max_page_size) {
    const auto old_buffer_size = page_array_[0].buffer().size();
    const auto old_capacity = page_array_[0].capacity();

    auto new_buffer_size = old_buffer_size * 2;
    auto new_capacity = old_capacity * 2;

    if (new_buffer_size > max_page_size ||
        max_page_size - new_buffer_size < descriptor_->size()) {
      new_buffer_size = max_page_size;
      new_capacity = max_page_size / descriptor_->size();
    }

    page_array_[0].Reserve(new_buffer_size);
    if (new_capacity > old_capacity) {
      for (auto i = old_capacity; i < new_capacity; ++i) {
        sparse_[0].Push(-1);
        hole_.Push(i);
      }
      return;
    }
  }

  MIRAGE_DCHECK(page_pool_);
  auto page = page_pool_->Allocate(descriptor_->align());
  page.Initialize(descriptor_.Clone());

  Array<size_t> sparse;
  Array<size_t> dense;
  sparse.Reserve(page.capacity());
  dense.Reserve(page.capacity());

  page_array_.Emplace(std::move(page));

  for (size_t i = 0; i < sparse.capacity(); ++i) {
    sparse.Push(kInvalidSparseId);
    hole_.Push(size_ + i);
  }
  sparse_.Emplace(std::move(sparse));
  dense_.Emplace(std::move(dense));
}

Archetype::Route Archetype::GetRoute(size_t sparse_id) const {
  const auto page_capacity = page_array_[0].capacity();

  const auto sparse_array_id = sparse_id / page_capacity;
  const auto sparse_offset = sparse_id - (sparse_array_id * page_capacity);
  const auto dense_id = sparse_[sparse_array_id][sparse_offset];

  const auto dense_array_id = dense_id / page_capacity;
  const auto dense_offset = dense_id - (dense_array_id * page_capacity);
  return {dense_array_id, dense_offset};
}
