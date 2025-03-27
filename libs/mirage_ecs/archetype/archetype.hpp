#ifndef MIRAGE_ECS_ARCHETYPE
#define MIRAGE_ECS_ARCHETYPE

#include "mirage_base/container/array.hpp"
#include "mirage_ecs/archetype/type_meta.hpp"
#include "mirage_ecs/define.hpp"

namespace mirage::ecs {

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
  AddTypeTo<Ts...>(descriptor);
  return descriptor;
}

template <typename T, typename... Ts>
void Archetype::Descriptor::AddTypeTo(Descriptor &descriptor) {
  descriptor.AddType<T>();
  AddTypeTo<Ts...>(descriptor);
}

template <typename T>
void Archetype::Descriptor::AddType() {
  const auto iter = type_array_.begin();
  while (iter != type_array_.end()) {
    if ((*iter)->GetTypeId() == TypeMeta::Of<T>()->GetTypeId()) {
      return;
    }
  }
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_ARCHETYPE
