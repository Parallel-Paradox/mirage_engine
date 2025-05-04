#ifndef MIRAGE_ECS_ENTITY_DATA_CHUNK
#define MIRAGE_ECS_ENTITY_DATA_CHUNK

#include <cstddef>

#include "mirage_base/auto_ptr/shared.hpp"
#include "mirage_ecs/entity/entity_layout.hpp"
#include "mirage_ecs/util/type_id.hpp"

namespace mirage::ecs {

class EntityChunk {
 public:
  EntityChunk() = delete;
  MIRAGE_ECS ~EntityChunk();

  EntityChunk(const EntityChunk &) = delete;
  EntityChunk &operator=(const EntityChunk &) = delete;

  MIRAGE_ECS EntityChunk(EntityChunk &&other) noexcept;
  MIRAGE_ECS EntityChunk &operator=(EntityChunk &&other) noexcept;

  MIRAGE_ECS EntityChunk(base::SharedLocal<EntityLayout> &&entity_layout,
                         size_t capacity);

  [[nodiscard]] MIRAGE_ECS const EntityLayout &entity_layout() const;

  [[nodiscard]] MIRAGE_ECS size_t byte_size() const;
  [[nodiscard]] MIRAGE_ECS std::byte *raw_ptr() const;

  [[nodiscard]] MIRAGE_ECS size_t capacity() const;
  [[nodiscard]] MIRAGE_ECS size_t size() const;

 private:
  base::SharedLocal<EntityLayout> entity_layout_{nullptr};

  size_t byte_size_{0};
  std::byte *raw_ptr_{nullptr};

  size_t capacity_{0};
  size_t size_{0};
};

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ENTITY_DATA_CHUNK
