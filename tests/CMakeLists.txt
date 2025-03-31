include(FetchContent)

message(STATUS "Introducing GTest...")
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.x)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # Set as shared library since there are multiple test suites.
  # MSVC can't export main.
  set(BUILD_SHARED_LIBS on)
  add_definitions("-DGTEST_CREATE_SHARED_LIBRARY=1")
  add_definitions("-DGTEST_LINKED_AS_SHARED_LIBRARY=1")
endif ()
FetchContent_MakeAvailable(googletest)

link_libraries(GTest::gtest_main)
enable_testing()
include(GoogleTest)

message(STATUS "Building test.mirage_base...")
add_executable(test.mirage_base
    mirage_base/auto_ptr/owned_tests.cpp
    mirage_base/container/array_tests.cpp
    mirage_base/container/hash_map_tests.cpp
    mirage_base/container/hash_set_tests.cpp
    mirage_base/container/singly_linked_list.cpp
    mirage_base/util/optional_tests.cpp)
gtest_discover_tests(test.mirage_base)
if (MIRAGE_BUILD_SPLIT)
  target_link_libraries(test.mirage_base mirage_base)
else ()
  target_link_libraries(test.mirage_base mirage_engine)
endif ()

message(STATUS "Building test.mirage_ecs...")
add_executable(test.mirage_ecs
    mirage_ecs/archetype/archetype_tests.cpp
    mirage_ecs/archetype/type_meta_tests.cpp
    mirage_ecs/system/query_tests.cpp)
gtest_discover_tests(test.mirage_ecs)
if (MIRAGE_BUILD_SPLIT)
  target_link_libraries(test.mirage_ecs mirage_ecs)
else ()
  target_link_libraries(test.mirage_ecs mirage_engine)
endif ()
