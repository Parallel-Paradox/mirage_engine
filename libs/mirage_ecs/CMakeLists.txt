message(STATUS "Building mirage_ecs...")

set(SRC
    "entity/archetype.cpp"
    "entity/data_chunk.cpp"
    "entity/entity_layout.cpp"
    "framework/plugin.cpp"
    "framework/world.cpp"
    "system/system_context.cpp"
    "system/system.cpp"
    "util/type_id.cpp"
    "util/type_set.cpp")

if (MIRAGE_BUILD_SPLIT)
  add_library(mirage_ecs SHARED ${SRC})
  target_compile_definitions(mirage_ecs PRIVATE MIRAGE_BUILD_ECS)
  target_link_libraries(mirage_ecs
      PRIVATE mirage_base
      PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
else ()
  target_sources(mirage_engine PRIVATE ${SRC})
endif ()
