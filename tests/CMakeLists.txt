include(FetchContent)

message(STATUS "Introducing GTest...")
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.x)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
link_libraries(GTest::gtest_main)
enable_testing()
include(GoogleTest)

message(STATUS "Building test.mirage_base...")
add_executable(test.mirage_base
    mirage_base/auto_ptr/owned_tests.cpp
    mirage_base/container/array_tests.cpp
    mirage_base/container/singly_linked_list.cpp
    mirage_base/util/optional_tests.cpp
)
gtest_discover_tests(test.mirage_base)
target_link_libraries(test.mirage_base mirage_base)
