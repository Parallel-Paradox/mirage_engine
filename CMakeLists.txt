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
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# --- Build mirage engine ---

if (CMAKE_BUILD_TYPE STREQUAL Debug)
  add_compile_definitions(MIRAGE_BUILD_DEBUG)
endif ()

option(MIRAGE_BUILD_SHARED "Build shared mirage engine" ON)
if (MIRAGE_BUILD_SHARED)
  add_compile_definitions(MIRAGE_BUILD_SHARED)
endif ()

include_directories(libs)

add_subdirectory(libs/mirage_base)
add_subdirectory(libs/mirage_ecs)

# Build examples and tests if library standalone
string(COMPARE EQUAL
    "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}" MIRAGE_STANDALONE)
if (MIRAGE_STANDALONE)
  add_subdirectory(tests)
endif ()
