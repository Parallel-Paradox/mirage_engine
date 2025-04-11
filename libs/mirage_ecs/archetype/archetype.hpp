#ifndef MIRAGE_ECS_ARCHETYPE_ARCHETYPE
#define MIRAGE_ECS_ARCHETYPE_ARCHETYPE

#include "mirage_base/container/array.hpp"
#include "mirage_base/container/hash_map.hpp"
#include "mirage_base/util/hash.hpp"
#include "mirage_ecs/define.hpp"
#include "mirage_ecs/util/type_id.hpp"

namespace mirage {
namespace ecs {

class Archetype {
 public:
  class Descriptor {
   public:
    MIRAGE_ECS Descriptor() = default;
    MIRAGE_ECS ~Descriptor() = default;

    MIRAGE_ECS Descriptor(Descriptor &&other) noexcept;
    MIRAGE_ECS Descriptor &operator=(Descriptor &&other) noexcept;

    MIRAGE_ECS [[nodiscard]] Descriptor Clone() const;

    template <typename... Ts>
    static Descriptor New();

    template <typename T, typename... Ts>
    static void AddTypeTo(Descriptor &descriptor);

    template <typename T>
    void AddType();
    MIRAGE_ECS void AddTypeId(TypeId type_id);

    [[nodiscard]] MIRAGE_ECS bool With(const Descriptor &desc) const;
    [[nodiscard]] MIRAGE_ECS bool Without(const Descriptor &desc) const;

    [[nodiscard]] MIRAGE_ECS const base::Array<TypeId> &GetTypeArray() const;
    [[nodiscard]] MIRAGE_ECS size_t GetMask() const;

    MIRAGE_ECS bool operator==(const Descriptor &other) const;

   private:
    base::Array<TypeId> type_array_{};
    size_t mask_{0};
  };

  MIRAGE_ECS Archetype() = default;
  MIRAGE_ECS ~Archetype() = default;

  MIRAGE_ECS Archetype(const Archetype &) = delete;
  MIRAGE_ECS Archetype &operator=(const Archetype &) = delete;

  MIRAGE_ECS Archetype(Archetype &&other) noexcept = default;
  MIRAGE_ECS Archetype &operator=(Archetype &&other) noexcept = default;

  template <typename... Ts>
  static Archetype New();
  explicit MIRAGE_ECS Archetype(Descriptor &&descriptor);

  [[nodiscard]] MIRAGE_ECS const Descriptor &GetDescriptor() const;

 private:
  // TODO
  base::HashMap<TypeId, size_t> offset_map_{};
  Descriptor descriptor_;
};

template <typename... Ts>
Archetype::Descriptor Archetype::Descriptor::New() {
  Descriptor descriptor;
  descriptor.type_array_.Reserve(sizeof...(Ts));
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
  AddTypeId(TypeId::Of<T>());
}

template <typename... Ts>
Archetype Archetype::New() {
  Archetype archetype;
  archetype.descriptor_ = Descriptor::New<Ts...>();
  return archetype;
}

}  // namespace ecs

template <>
struct base::Hash<ecs::Archetype::Descriptor> {
  size_t operator()(const ecs::Archetype::Descriptor &descriptor) const {
    return descriptor.GetMask();
  }
};

}  // namespace mirage

#endif  // MIRAGE_ECS_ARCHETYPE_ARCHETYPE
