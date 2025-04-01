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
  class Descriptor {
   public:
    MIRAGE_ECS Descriptor() = default;
    MIRAGE_ECS ~Descriptor() = default;

    template <typename... Ts>
    static Descriptor New();

    template <typename T, typename... Ts>
    static void AddTypeTo(Descriptor &descriptor);

    template <typename T>
    void AddType();
    void AddTypeMeta(const TypeMeta *type_meta);

    [[nodiscard]] bool With(const Descriptor &desc) const;
    [[nodiscard]] bool Without(const Descriptor &desc) const;

    [[nodiscard]] MIRAGE_ECS const base::Array<const TypeMeta *> &GetTypeArray()
        const;
    [[nodiscard]] MIRAGE_ECS size_t GetMask() const;

    MIRAGE_ECS bool operator==(const Descriptor &other) const;

   private:
    base::Array<const TypeMeta *> type_array_{};
    size_t mask_{0};
  };

  Archetype() = default;
  ~Archetype() = default;

  Archetype(const Archetype &) = delete;
  Archetype &operator=(const Archetype &) = delete;

  Archetype(Archetype &&other) noexcept;
  Archetype &operator=(Archetype &&other) = default;

  template <typename... Ts>
  static Archetype New();

  [[nodiscard]] const Descriptor &GetDescriptor() const;

 private:
  Descriptor descriptor_;
};

template <typename... Ts>
Archetype::Descriptor Archetype::Descriptor::New() {
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
  AddTypeMeta(TypeMeta::Of<T>());
}

template <typename... Ts>
Archetype Archetype::New() {
  Archetype archetype;
  archetype.descriptor_ = Descriptor::New<Ts...>();
  return archetype;
}

}  // namespace ecs

template <>
struct base::Hash<const ecs::Archetype::Descriptor> {
  size_t operator()(const ecs::Archetype::Descriptor &descriptor) const {
    return descriptor.GetMask();
  }
};

}  // namespace mirage

#endif  // MIRAGE_ECS_ARCHETYPE
