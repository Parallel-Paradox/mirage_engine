# --- Project settings ---

cmake_minimum_required(VERSION 3.28)
message(STATUS "CMake Version: ${CMAKE_VERSION}")

project(mirage_engine)

set(CMAKE_CXX_STANDARD 20)

message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(/W4 /WX)
  add_compile_definitions(MIRAGE_BUILD_MSVC)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
else ()
  message(FATAL_ERROR "Unsupported compiler.")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

# --- Options ---

option(MIRAGE_BUILD_SHARED "Build shared mirage engine" ON)
option(MIRAGE_BUILD_SPLIT
    "Build split mirage engine, depend on MIRAGE_BUILD_SHARED" ON)
if (MIRAGE_BUILD_SPLIT AND NOT MIRAGE_BUILD_SHARED)
  message(FATAL_ERROR
      "MIRAGE_BUILD_SPLIT must be OFF when MIRAGE_BUILD_SHARED is OFF")
endif ()

option(MIRAGE_BUILD_TESTS "Build mirage engine tests" ON)

# --- Dependencies ---

include(FetchContent)

message(STATUS "Introducing spdlog...")
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.1)
if (MIRAGE_BUILD_SPLIT)
  set(SPDLOG_BUILD_SHARED ON)
endif ()
FetchContent_MakeAvailable(spdlog)

# --- Build mirage engine ---

if (CMAKE_BUILD_TYPE STREQUAL Debug)
  add_compile_definitions(MIRAGE_BUILD_DEBUG)
endif ()

if (MIRAGE_BUILD_SHARED)
  message(STATUS "Build shared mirage engine...")
  add_compile_definitions(MIRAGE_BUILD_SHARED)
endif ()

if (NOT MIRAGE_BUILD_SPLIT)
  if (MIRAGE_BUILD_SHARED)
    add_library(mirage_engine SHARED)
  else ()
    add_library(mirage_engine STATIC)
  endif ()
  target_compile_definitions(mirage_engine PRIVATE MIRAGE_BUILD_ENGINE)
  target_link_libraries(mirage_engine
      PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
endif ()

include_directories(libs)

add_subdirectory(libs/mirage_base)
add_subdirectory(libs/mirage_ecs)

# Build tests
if (MIRAGE_BUILD_TESTS)
  message(STATUS "Build mirage engine tests...")
  add_subdirectory(tests)
endif ()
