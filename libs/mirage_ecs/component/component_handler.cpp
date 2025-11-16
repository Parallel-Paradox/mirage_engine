#include "mirage_ecs/component/component_handler.hpp"

#include "mirage_base/util/type_id.hpp"

using namespace mirage::base;
using namespace mirage::ecs;

ComponentHandler::ComponentHandler(HandlerFuncPtr handler)
    : handler_(handler) {}

bool ComponentHandler::operator==(const ComponentHandler& other) const {
  return type_id() == other.type_id();
}

bool ComponentHandler::operator!=(const ComponentHandler& other) const {
  return !(*this == other);
}

std::strong_ordering ComponentHandler::operator<=>(
    const ComponentHandler& other) const {
  return type_id() <=> other.type_id();
}

TypeId ComponentHandler::type_id() const {
  return TypeId(
      *static_cast<const TypeMeta*>(handler_(kTypeMeta, nullptr, nullptr)));
}

void ComponentHandler::move(void* target, void* dest) const {
  handler_(kMove, target, dest);
}

void ComponentHandler::destruct(void* target) const {
  handler_(kDestruct, target, nullptr);
}
