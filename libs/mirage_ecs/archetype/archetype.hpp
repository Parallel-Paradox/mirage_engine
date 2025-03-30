#ifndef MIRAGE_ECS_ARCHETYPE
#define MIRAGE_ECS_ARCHETYPE

#include "mirage_base/container/array.hpp"
#include "mirage_base/util/hash.hpp"
#include "mirage_ecs/archetype/type_meta.hpp"
#include "mirage_ecs/define.hpp"

namespace mirage {
namespace ecs {

class MIRAGE_ECS Archetype {
 public:
  class Descriptor;
};

class Archetype::Descriptor {
 public:
  MIRAGE_ECS Descriptor() = default;
  MIRAGE_ECS ~Descriptor() = default;

  template <typename... Ts>
  static Descriptor Build();

  template <typename T, typename... Ts>
  static void AddTypeTo(Descriptor &descriptor);

  template <typename T>
  void AddType();

  [[nodiscard]] MIRAGE_ECS const base::Array<const TypeMeta *> &GetTypeArray()
      const;
  [[nodiscard]] MIRAGE_ECS size_t GetHash() const;

  MIRAGE_ECS bool operator==(const Descriptor &other) const;

 private:
  base::Array<const TypeMeta *> type_array_;
  size_t hash_{0};
};

template <typename... Ts>
Archetype::Descriptor Archetype::Descriptor::Build() {
  Descriptor descriptor;
  if constexpr (sizeof...(Ts) > 0) AddTypeTo<Ts...>(descriptor);
  return descriptor;
}

template <typename T, typename... Ts>
void Archetype::Descriptor::AddTypeTo(Descriptor &descriptor) {
  descriptor.AddType<T>();
  if constexpr (sizeof...(Ts) > 0) AddTypeTo<Ts...>(descriptor);
}

template <typename T>
void Archetype::Descriptor::AddType() {
  const TypeMeta *type_meta = TypeMeta::Of<T>();

  auto iter = type_array_.begin();
  while (iter != type_array_.end()) {
    const TypeMeta *iter_type_meta = *iter;
    if (iter_type_meta->GetTypeId() == type_meta->GetTypeId()) {
      return;
    }
    if (iter_type_meta->GetTypeId() > type_meta->GetTypeId()) break;
    ++iter;
  }
  hash_ |= static_cast<size_t>(1) << (type_meta->GetTypeId() % 64);
  if (iter != type_array_.end()) {
    type_array_.Insert(iter, type_meta);
  } else {
    type_array_.Emplace(type_meta);
  }
}

}  // namespace ecs

template <>
struct base::Hash<const ecs::Archetype::Descriptor> {
  size_t operator()(const ecs::Archetype::Descriptor &descriptor) const {
    return descriptor.GetHash();
  }
};

}  // namespace mirage

#endif  // MIRAGE_ECS_ARCHETYPE
