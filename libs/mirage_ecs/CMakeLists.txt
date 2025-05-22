message(STATUS "Building mirage_ecs...")

file(GLOB_RECURSE SRC "**.cpp")

if (MIRAGE_BUILD_SPLIT)
  add_library(mirage_ecs SHARED ${SRC})
  target_compile_definitions(mirage_ecs PRIVATE MIRAGE_BUILD_ECS)
  target_link_libraries(mirage_ecs
      PUBLIC mirage_base
      PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
else ()
  target_sources(mirage_engine PRIVATE ${SRC})
  target_compile_definitions(mirage_engine PRIVATE MIRAGE_BUILD_ECS)
endif ()
