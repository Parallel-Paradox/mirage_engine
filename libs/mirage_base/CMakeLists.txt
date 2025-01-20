message(STATUS "Building mirage_base...")

set(SRC ./sync/lock.cpp)
if (MSVC)
  set(SRC ${SRC} ./sync/lock_impl_msvc.cpp)
else ()
  set(SRC ${SRC} ./sync/lock_impl_posix.cpp)
endif ()

if (MIRAGE_BUILD_SHARED)
  add_library(mirage_base SHARED ${SRC})
else ()
  add_library(mirage_base STATIC ${SRC})
endif ()

target_compile_definitions(mirage_base PRIVATE MIRAGE_BUILD_BASE)
