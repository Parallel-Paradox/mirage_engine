#ifndef MIRAGE_ECS_COMPONENT_COMPONENT_HANDLER
#define MIRAGE_ECS_COMPONENT_COMPONENT_HANDLER

#include "mirage_base/util/hash.hpp"
#include "mirage_base/util/type_id.hpp"
#include "mirage_ecs/define/export.hpp"
#include "mirage_ecs/util/marker.hpp"

namespace mirage {
namespace ecs {

class MIRAGE_ECS ComponentHandler {
 public:
  ComponentHandler() = delete;
  ~ComponentHandler() = default;

  ComponentHandler(const ComponentHandler &) = default;
  ComponentHandler &operator=(const ComponentHandler &) = default;

  enum Action {
    kMove,
    kDestruct,
    kTypeMeta,
  };

  using HandlerFuncPtr = void *(*)(Action action, void *target, void *dest);

  ComponentHandler(HandlerFuncPtr handler);

  template <IsComponent T>
  static ComponentHandler Of() {
    return ComponentHandler(Handler<T>);
  }

  bool operator==(const ComponentHandler &other) const;
  bool operator!=(const ComponentHandler &other) const;
  std::strong_ordering operator<=>(const ComponentHandler &other) const;

  [[nodiscard]] base::TypeId type_id() const;
  void move(void *target, void *dest) const;
  void destruct(void *target) const;

 private:
  template <IsComponent T>
  static void *Handler(Action action, void *target, void *dest) {
    T *target_ptr = reinterpret_cast<T *>(target);
    T *dest_ptr = reinterpret_cast<T *>(dest);
    switch (action) {
      case kMove:
        new (dest_ptr) T(std::move(*target_ptr));
        target_ptr->~T();
        break;
      case kDestruct:
        target_ptr->~T();
        break;
      case kTypeMeta:
        return const_cast<base::TypeMeta *>(&base::TypeMeta::Of<T>());
    }
    return nullptr;
  }

  HandlerFuncPtr handler_{nullptr};
};

using ComponentId = ComponentHandler;

}  // namespace ecs

template <>
struct base::Hash<ecs::ComponentHandler> {
  size_t operator()(const ecs::ComponentHandler &handler) const {
    return handler.type_id().hash_code();
  }

  size_t operator()(const base::TypeId &type_id) const {
    return type_id.hash_code();
  }
};

}  // namespace mirage

#endif  // MIRAGE_ECS_COMPONENT_COMPONENT_HANDLER
