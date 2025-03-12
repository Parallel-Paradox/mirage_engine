message(STATUS "Building mirage_ecs...")

set(SRC
    archetype/type_meta.cpp
    archetype/archetype.cpp)

if (MIRAGE_BUILD_SHARED)
  add_library(mirage_ecs SHARED ${SRC})
else ()
  add_library(mirage_ecs STATIC ${SRC})
endif ()

target_compile_definitions(mirage_ecs PRIVATE MIRAGE_BUILD_ECS)

target_link_libraries(mirage_ecs
    PRIVATE mirage_base
    PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
