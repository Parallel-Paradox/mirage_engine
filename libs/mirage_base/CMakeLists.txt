message(STATUS "Building mirage_base...")

set(SRC
    auto_ptr/ref_count.cpp
    sync/lock.cpp)
if (MSVC)
  set(SRC ${SRC} sync/lock_impl_msvc.cpp)
else ()
  set(SRC ${SRC} sync/lock_impl_posix.cpp)
endif ()

if (MIRAGE_BUILD_SPLIT)
  add_library(mirage_base SHARED ${SRC})
  target_compile_definitions(mirage_base PRIVATE MIRAGE_BUILD_BASE)
else ()
  target_sources(mirage_engine PRIVATE ${SRC})
endif ()
