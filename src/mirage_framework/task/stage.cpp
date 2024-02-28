#include "mirage_framework/task/stage.hpp"

using namespace mirage;

Stage::Stage(Stage&& other)
    : prev_(std::move(other.prev_)), next_(std::move(other.next_)) {}
