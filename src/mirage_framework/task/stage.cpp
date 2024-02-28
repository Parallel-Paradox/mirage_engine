#include "mirage_framework/task/stage.hpp"

using namespace mirage;

INST_ARRAY(WeakAsync<Stage>);
INST_ARRAY(SharedAsync<Stage>);

Stage::Stage(Stage&& other)
    : prev_(std::move(other.prev_)), next_(std::move(other.next_)) {}
