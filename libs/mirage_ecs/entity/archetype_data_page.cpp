#include "mirage_ecs/entity/archetype_data_page.hpp"

#include <cstddef>

#include "mirage_ecs/component/component_id.hpp"

using namespace mirage::ecs;

using SharedDescriptor = ArchetypeDataPage::SharedDescriptor;
using Buffer = ArchetypeDataPage::Buffer;

ArchetypeDataPage::ArchetypeDataPage(size_t buffer_size, size_t align)
    : buffer_(buffer_size, align) {}

ArchetypeDataPage::~ArchetypeDataPage() { Reset(); }

void ArchetypeDataPage::Reset() {
  Clear();
  descriptor_ = nullptr;
  capacity_ = 0;
  size_ = 0;
}

void ArchetypeDataPage::Clear() {
  if (!is_initialized()) {
    return;
  }
  for (size_t i = 0; i < size_; ++i) {
    std::byte* begin = buffer_.ptr() + i * descriptor_->size();
    for (const auto& entry : descriptor_->offset_map()) {
      entry.key().destruct_func()(begin + entry.val());
    }
  }
}

bool ArchetypeDataPage::is_initialized() const {
  return descriptor_ != nullptr;
}

const SharedDescriptor& ArchetypeDataPage::descriptor() const {
  return descriptor_;
}

size_t ArchetypeDataPage::capacity() const { return capacity_; }

size_t ArchetypeDataPage::size() const { return size_; }

const Buffer& ArchetypeDataPage::buffer() const { return buffer_; }

void* ArchetypeDataPage::View::TryGetImpl(ComponentId id) const {
  const auto& offset_map = descriptor_->offset_map();
  auto iter = offset_map.TryFind(id);
  if (iter == offset_map.end()) {
    return nullptr;
  }
  return view_ptr_ + iter->val();
}
