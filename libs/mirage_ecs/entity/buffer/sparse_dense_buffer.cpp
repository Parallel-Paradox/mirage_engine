#include "mirage_ecs/entity/buffer/sparse_dense_buffer.hpp"

#include "mirage_base/define/check.hpp"

using namespace mirage::ecs;

DenseBuffer::DenseBuffer(Buffer&& buffer)
    : buffer_(std::move(buffer)),
      capacity_(static_cast<uint16_t>(buffer_.size()) / kUnitSize) {
  MIRAGE_DCHECK(buffer_.align() >= DenseBuffer::kMinAlign);
  MIRAGE_DCHECK(buffer_.size() <= DenseBuffer::kMaxBufferSize);
}

DenseBuffer::~DenseBuffer() { std::move(*this).TakeBuffer(); }

DenseBuffer::DenseBuffer(DenseBuffer&& other) noexcept
    : buffer_(std::move(other.buffer_)),
      size_(other.size_),
      capacity_(other.capacity_) {
  other.size_ = 0;
  other.capacity_ = 0;
}

DenseBuffer& DenseBuffer::operator=(DenseBuffer&& other) noexcept {
  if (this != &other) {
    this->~DenseBuffer();
    new (this) DenseBuffer(std::move(other));
  }
  return *this;
}

SparseId const& DenseBuffer::operator[](const uint16_t index) const {
  MIRAGE_DCHECK(index < size_);
  return reinterpret_cast<const SparseId*>(buffer_.ptr())[index];
}

SparseId& DenseBuffer::operator[](const uint16_t index) {
  MIRAGE_DCHECK(index < size_);
  return reinterpret_cast<SparseId*>(buffer_.ptr())[index];
}

void DenseBuffer::Push(const SparseId sparse_id) {
  MIRAGE_DCHECK(size_ < capacity_);
  auto* id_begin_ptr = reinterpret_cast<SparseId*>(buffer_.ptr());
  id_begin_ptr[size_] = sparse_id;
  ++size_;
}

void DenseBuffer::RemoveTail() {
  MIRAGE_DCHECK(size_ > 0);
  --size_;
  auto* id_begin_ptr = reinterpret_cast<SparseId*>(buffer_.ptr());
  id_begin_ptr[size_] = kInvalidSparseId;
}

void DenseBuffer::Reserve(const uint16_t capacity) {
  if (capacity <= capacity_) {
    return;
  }

  DenseBuffer old_buffer = std::move(*this);
  const auto old_buffer_size = old_buffer.size_;
  new (this) DenseBuffer({capacity * kUnitSize, old_buffer.buffer_.align()});
  for (auto i = 0; i < old_buffer_size; ++i) {
    Push(old_buffer[i]);
  }
}

DenseBuffer::Buffer DenseBuffer::TakeBuffer() && {
  size_ = 0;
  capacity_ = 0;
  return std::move(buffer_);
}

uint16_t DenseBuffer::size() const { return size_; }

uint16_t DenseBuffer::capacity() const { return capacity_; }

bool DenseBuffer::is_full() const { return size_ == capacity_; }

SparseBuffer::SparseBuffer(Buffer&& buffer) : buffer_(std::move(buffer)) {
  MIRAGE_DCHECK(buffer_.align() >= SparseBuffer::kMinAlign);
  MIRAGE_DCHECK(buffer_.size() <= SparseBuffer::kMaxBufferSize);

  const auto hole_cnt = buffer_.size() / kUnitSize;
  hole_cnt_ = static_cast<uint16_t>(hole_cnt);
  capacity_ = hole_cnt_;

  auto* id_begin_ptr = reinterpret_cast<DenseId*>(buffer_.ptr());
  auto* hole_end_ptr =
      reinterpret_cast<uint16_t*>(buffer_.ptr() + buffer_.size());
  for (auto i = 0; i < hole_cnt_; ++i) {
    *id_begin_ptr = kInvalidDenseId;
    ++id_begin_ptr;

    --hole_end_ptr;
    *hole_end_ptr = i;
  }
}

SparseBuffer::~SparseBuffer() { std::move(*this).TakeBuffer(); }

SparseBuffer::SparseBuffer(SparseBuffer&& other) noexcept
    : buffer_(std::move(other.buffer_)),
      size_(other.size_),
      hole_cnt_(other.hole_cnt_),
      capacity_(other.capacity_) {
  other.size_ = 0;
  other.hole_cnt_ = 0;
  other.capacity_ = 0;
}

SparseBuffer& SparseBuffer::operator=(SparseBuffer&& other) noexcept {
  if (this != &other) {
    this->~SparseBuffer();
    new (this) SparseBuffer(std::move(other));
  }
  return *this;
}

DenseId const& SparseBuffer::operator[](const uint16_t index) const {
  return reinterpret_cast<const DenseId*>(buffer_.ptr())[index];
}

DenseId& SparseBuffer::operator[](const uint16_t index) {
  return reinterpret_cast<DenseId*>(buffer_.ptr())[index];
}

uint16_t SparseBuffer::FillHole(const DenseId dense_id) {
  MIRAGE_DCHECK(hole_cnt_ > 0);

  auto* id_begin_ptr = reinterpret_cast<DenseId*>(buffer_.ptr());
  const auto* hole_ptr =
      reinterpret_cast<uint16_t*>(buffer_.ptr() + buffer_.size()) - size_ - 1;

  id_begin_ptr[*hole_ptr] = dense_id;
  --hole_cnt_;
  ++size_;
  return *hole_ptr;
}

DenseId SparseBuffer::Remove(const uint16_t index) {
  MIRAGE_DCHECK(size_ > 0);
  auto* hole_ptr =
      reinterpret_cast<uint16_t*>(buffer_.ptr() + buffer_.size()) - size_;
  *hole_ptr = index;

  auto* id_begin_ptr = reinterpret_cast<DenseId*>(buffer_.ptr());
  auto& id = id_begin_ptr[index];
  const auto rv = id;
  MIRAGE_DCHECK(rv != kInvalidDenseId);
  id = kInvalidDenseId;

  ++hole_cnt_;
  --size_;

  return rv;
}

void SparseBuffer::Reserve(const uint16_t capacity) {
  if (capacity <= capacity_) {
    return;
  }

  Buffer new_buffer({capacity * kUnitSize, buffer_.align()});

  auto* new_id_begin_ptr = reinterpret_cast<DenseId*>(new_buffer.ptr());
  const auto* id_begin_ptr = reinterpret_cast<DenseId*>(buffer_.ptr());
  for (auto i = 0; i < capacity_; ++i) {
    *new_id_begin_ptr = *id_begin_ptr;
    ++new_id_begin_ptr;
    ++id_begin_ptr;
  }
  for (auto i = capacity_; i < capacity; ++i) {
    *new_id_begin_ptr = kInvalidDenseId;
    ++new_id_begin_ptr;
  }

  auto* new_hole_end_ptr =
      reinterpret_cast<uint16_t*>(new_buffer.ptr() + new_buffer.size());
  auto* hole_end_ptr =
      reinterpret_cast<uint16_t*>(buffer_.ptr() + buffer_.size());
  for (auto i = 0; i < capacity_; ++i) {
    --new_hole_end_ptr;
    --hole_end_ptr;
    *new_hole_end_ptr = *hole_end_ptr;
  }
  for (auto i = capacity_; i < capacity; ++i) {
    --new_hole_end_ptr;
    *new_hole_end_ptr = i;
  }

  buffer_ = std::move(new_buffer);
  capacity_ = capacity;
  hole_cnt_ = capacity_ - size_;
}

SparseBuffer::Buffer SparseBuffer::TakeBuffer() && {
  size_ = 0;
  hole_cnt_ = 0;
  capacity_ = 0;
  return std::move(buffer_);
}

uint16_t SparseBuffer::size() const { return size_; }

uint16_t SparseBuffer::hole_cnt() const { return hole_cnt_; }

uint16_t SparseBuffer::capacity() const { return capacity_; }
