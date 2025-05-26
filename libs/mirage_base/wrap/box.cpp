#include "mirage_base/wrap/box.hpp"

#include "mirage_base/define/check.hpp"
#include "mirage_base/util/type_id.hpp"

using namespace mirage::base;

Box::~Box() { Reset(); }

Box::Box(Box&& other) noexcept {
  if (other.handle_func_ != nullptr) {
    other.Call(kMove, this);
  }
}

Box& Box::operator=(Box&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  this->~Box();
  new (this) Box(std::move(other));
  return *this;
}

void Box::Reset() {
  if (handle_func_ == nullptr) return;
  Call(kDestruct);
  obj_.ptr = nullptr;
  handle_func_ = nullptr;
}

bool Box::is_valid() const { return handle_func_ != nullptr; }

TypeId Box::type_id() const {
  MIRAGE_DCHECK(is_valid());
  return TypeId(*static_cast<const TypeMeta*>(Call(kTypeMeta)));
}

void* Box::raw_ptr() const { return Call(kGet); }

void* Box::Call(Action action, Box* dest, const TypeMeta* type_meta) const {
  return handle_func_(action, const_cast<Box*>(this), dest, type_meta);
}
