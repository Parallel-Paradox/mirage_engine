#ifndef MIRAGE_ECS_ARCHETYPE_ARCHETYPE_DESCRIPTOR
#define MIRAGE_ECS_ARCHETYPE_ARCHETYPE_DESCRIPTOR

#include "mirage_base/container/array.hpp"
#include "mirage_ecs/define.hpp"
#include "mirage_ecs/util/type_id.hpp"

namespace mirage {
namespace ecs {

class ArchetypeDescriptor {
 public:
  MIRAGE_ECS ArchetypeDescriptor() = default;
  MIRAGE_ECS ~ArchetypeDescriptor() = default;

  MIRAGE_ECS ArchetypeDescriptor(ArchetypeDescriptor &&other) noexcept;
  MIRAGE_ECS ArchetypeDescriptor &operator=(
      ArchetypeDescriptor &&other) noexcept;

  MIRAGE_ECS [[nodiscard]] ArchetypeDescriptor Clone() const;

  template <typename... Ts>
  static ArchetypeDescriptor New();

  template <typename T, typename... Ts>
  static void AddTypeTo(ArchetypeDescriptor &descriptor);

  template <typename T>
  void AddType();
  MIRAGE_ECS void AddTypeId(TypeId type_id);

  [[nodiscard]] MIRAGE_ECS bool With(const ArchetypeDescriptor &desc) const;
  [[nodiscard]] MIRAGE_ECS bool Without(const ArchetypeDescriptor &desc) const;

  [[nodiscard]] MIRAGE_ECS const base::Array<TypeId> &GetTypeArray() const;
  [[nodiscard]] MIRAGE_ECS size_t GetMask() const;

  MIRAGE_ECS bool operator==(const ArchetypeDescriptor &other) const;

 private:
  base::Array<TypeId> type_array_{};
  size_t mask_{0};
};

template <typename... Ts>
ArchetypeDescriptor ArchetypeDescriptor::New() {
  ArchetypeDescriptor descriptor;
  descriptor.type_array_.Reserve(sizeof...(Ts));
  if constexpr (sizeof...(Ts) > 0) AddTypeTo<Ts...>(descriptor);
  return descriptor;
}

template <typename T, typename... Ts>
void ArchetypeDescriptor::AddTypeTo(ArchetypeDescriptor &descriptor) {
  descriptor.AddType<T>();
  if constexpr (sizeof...(Ts) > 0) AddTypeTo<Ts...>(descriptor);
}

template <typename T>
void ArchetypeDescriptor::AddType() {
  AddTypeId(TypeId::Of<T>());
}

}  // namespace ecs

template <>
struct base::Hash<ecs::ArchetypeDescriptor> {
  size_t operator()(const ecs::ArchetypeDescriptor &descriptor) const {
    return descriptor.GetMask();
  }
};

}  // namespace mirage

#endif  // MIRAGE_ECS_ARCHETYPE_ARCHETYPE_DESCRIPTOR
