#ifndef MIRAGE_ECS_ENTITY_DATA_CHUNK
#define MIRAGE_ECS_ENTITY_DATA_CHUNK

#include <cstddef>

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_base/container/hash_map.hpp"
#include "mirage_ecs/util/type_id.hpp"

namespace mirage::ecs {

class DataChunk {
 public:
  struct Header {
    size_t entity_align{0};
    size_t entity_size{0};
    base::HashMap<TypeId, size_t> type_addr_offset_map;
  };

  MIRAGE_ECS DataChunk() = delete;
  MIRAGE_ECS ~DataChunk();

  DataChunk(const DataChunk &) = delete;
  DataChunk &operator=(const DataChunk &) = delete;

  MIRAGE_ECS DataChunk(DataChunk &&other) noexcept;
  MIRAGE_ECS DataChunk &operator=(DataChunk &&other) noexcept;

  MIRAGE_ECS DataChunk(base::SharedLocal<Header> &&header, size_t capacity);

  [[nodiscard]] MIRAGE_ECS const base::SharedLocal<Header> &header() const;

  [[nodiscard]] MIRAGE_ECS size_t byte_size() const;
  [[nodiscard]] MIRAGE_ECS std::byte *raw_ptr() const;

  [[nodiscard]] MIRAGE_ECS size_t capacity() const;
  [[nodiscard]] MIRAGE_ECS size_t size() const;

 private:
  base::SharedLocal<Header> header_;

  size_t byte_size_{0};
  std::byte *raw_ptr_{nullptr};

  size_t capacity_{0};
  size_t size_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_DATA_CHUNK
