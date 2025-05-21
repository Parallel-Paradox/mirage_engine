include(FetchContent)

message(STATUS "Introducing GTest...")
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0)
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

add_subdirectory("mirage_base")
add_subdirectory("mirage_ecs")
