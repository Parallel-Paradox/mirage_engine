#ifndef MIRAGE_ECS_FRAMEWORK_WORLD
#define MIRAGE_ECS_FRAMEWORK_WORLD

#include "mirage_base/util/type_id.hpp"
#include "mirage_base/wrap/optional.hpp"
#include "mirage_ecs/entity/entity_manager.hpp"
#include "mirage_ecs/util/marker.hpp"

namespace mirage::ecs {

class World {
  using TypeId = base::TypeId;

  template <typename T>
  using Optional = base::Optional<T>;

 public:
  using ResourceMap = base::HashMap<TypeId, BoxResource>;

  World() = default;
  ~World() = default;

  template <IsResource T, typename... Args>
  T& InitResource(Args&&... args);

  template <IsResource T, typename... Args>
  Optional<T> SetResource(Args&&... args);

  template <IsResource T>
  T* TryGetResource();

  template <IsResource T>
  T& GetResource();

 private:
  ResourceMap resource_map_;
  EntityManager entity_manager_;
};

template <IsResource T, typename... Args>
T& World::InitResource(Args&&... args) {
  T* resource_ptr = TryGetResource<T>();
  if (resource_ptr) {
    return *resource_ptr;
  }

  resource_map_.Insert(TypeId::Of<T>(),
                       BoxResource(T(std::forward<Args>(args)...)));
  return *resource_ptr;
}

template <IsResource T, typename... Args>
base::Optional<T> World::SetResource(Args&&... args) {
  Optional<base::HashKeyVal<TypeId, BoxResource>> optional_kv =
      resource_map_.Insert(TypeId::Of<T>(),
                           BoxResource(T(std::forward<Args>(args)...)));
  if (optional_kv.is_valid()) {
    return Optional<T>::None();
  }
  auto kv = optional_kv.Unwrap();
  T* resource = static_cast<T*>(kv.val().raw_ptr());
  return Optional<T>::New(std::move(*resource));
}

template <IsResource T>
T* World::TryGetResource() {
  ResourceMap::Iterator iter = resource_map_.TryFind(TypeId::Of<T>());
  if (iter == resource_map_.end()) {
    return nullptr;
  }
  return static_cast<T*>(iter->val().raw_ptr());
}

template <IsResource T>
T& World::GetResource() {
  return *TryGetResource<T>();
}

}  // namespace mirage::ecs

#endif  // MIRAGE_ECS_FRAMEWORK_WORLD
