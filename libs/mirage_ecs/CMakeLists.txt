message(STATUS "Building mirage_ecs...")

set(SRC
    archetype/archetype.cpp
    archetype/archetype_descriptor.cpp
    system/context.cpp
    util/type_id.cpp)

if (MIRAGE_BUILD_SPLIT)
  add_library(mirage_ecs SHARED ${SRC})
  target_compile_definitions(mirage_ecs PRIVATE MIRAGE_BUILD_ECS)
  target_link_libraries(mirage_ecs
      PRIVATE mirage_base
      PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
else ()
  target_sources(mirage_engine PRIVATE ${SRC})
endif ()
