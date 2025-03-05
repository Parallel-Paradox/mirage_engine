message(STATUS "Building mirage_ecs...")

set(SRC ./archetype/meta_type.cpp)

if (MIRAGE_BUILD_SHARED)
  add_library(mirage_ecs SHARED ${SRC})
else ()
  add_library(mirage_ecs STATIC ${SRC})
endif ()

target_compile_definitions(mirage_ecs PRIVATE MIRAGE_BUILD_ECS)
