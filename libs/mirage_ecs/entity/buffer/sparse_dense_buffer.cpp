#include "mirage_ecs/entity/buffer/sparse_dense_buffer.hpp"

#include "mirage_base/define/check.hpp"

using namespace mirage::ecs;

DenseBuffer::DenseBuffer(Buffer&& buffer)
    : buffer_(std::move(buffer)),
      capacity_(static_cast<uint16_t>(buffer_.size()) / sizeof(SparseId)) {
  MIRAGE_DCHECK(buffer_.align() >= DenseBuffer::kMinAlign);
  MIRAGE_DCHECK(buffer_.size() <= DenseBuffer::kMaxBufferSize);
}

DenseBuffer::~DenseBuffer() {
  size_ = 0;
  capacity_ = 0;
}

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

SparseId const& DenseBuffer::operator[](uint16_t index) const {
  MIRAGE_DCHECK(index < size_);
  return reinterpret_cast<const SparseId*>(buffer_.ptr())[index];
}

SparseId& DenseBuffer::operator[](uint16_t index) {
  MIRAGE_DCHECK(index < size_);
  return reinterpret_cast<SparseId*>(buffer_.ptr())[index];
}

void DenseBuffer::Push(SparseId sparse_id) {
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

uint16_t DenseBuffer::size() const { return size_; }

uint16_t DenseBuffer::capacity() const { return capacity_; }

SparseBuffer::SparseBuffer(Buffer&& buffer) : buffer_(std::move(buffer)) {
  MIRAGE_DCHECK(buffer_.align() >= SparseBuffer::kMinAlign);
  MIRAGE_DCHECK(buffer_.size() <= SparseBuffer::kMaxBufferSize);

  hole_cnt_ = buffer_.size() / (sizeof(DenseId) + sizeof(uint16_t));
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

SparseBuffer::~SparseBuffer() {
  size_ = 0;
  hole_cnt_ = 0;
  capacity_ = 0;
}

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

DenseId const& SparseBuffer::operator[](uint16_t index) const {
  return reinterpret_cast<const DenseId*>(buffer_.ptr())[index];
}

DenseId& SparseBuffer::operator[](uint16_t index) {
  return reinterpret_cast<DenseId*>(buffer_.ptr())[index];
}

uint16_t SparseBuffer::FillHole(DenseId dense_id) {
  MIRAGE_DCHECK(hole_cnt_ > 0);

  auto* id_begin_ptr = reinterpret_cast<DenseId*>(buffer_.ptr());
  auto* hole_ptr =
      reinterpret_cast<uint16_t*>(buffer_.ptr() + buffer_.size()) - size_ - 1;

  id_begin_ptr[*hole_ptr] = dense_id;
  --hole_cnt_;
  ++size_;
  return *hole_ptr;
}

DenseId SparseBuffer::Remove(uint16_t index) {
  MIRAGE_DCHECK(size_ > 0);
  auto* hole_ptr =
      reinterpret_cast<uint16_t*>(buffer_.ptr() + buffer_.size()) - size_;
  *hole_ptr = index;

  auto* id_begin_ptr = reinterpret_cast<DenseId*>(buffer_.ptr());
  auto& id = id_begin_ptr[index];
  auto rv = id;
  MIRAGE_DCHECK(rv != kInvalidDenseId);
  id = kInvalidDenseId;

  ++hole_cnt_;
  --size_;

  return rv;
}

uint16_t SparseBuffer::size() const { return size_; }

uint16_t SparseBuffer::hole_cnt() const { return hole_cnt_; }

uint16_t SparseBuffer::capacity() const { return capacity_; }
