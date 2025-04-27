#ifndef MIRAGE_ECS_ENTITY_DATA_CHUNK
#define MIRAGE_ECS_ENTITY_DATA_CHUNK

#include <cstddef>

namespace mirage::ecs {

class DataChunk {
 public:
  DataChunk() = default;
  ~DataChunk();

  DataChunk(const DataChunk &) = delete;
  DataChunk &operator=(const DataChunk &) = delete;

  DataChunk(DataChunk &&other) noexcept;
  DataChunk &operator=(DataChunk &&other) noexcept;

  [[nodiscard]] std::byte *raw_ptr() const;
  [[nodiscard]] size_t size() const;

 private:
  std::byte *raw_ptr_{nullptr};
  size_t size_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_DATA_CHUNK
