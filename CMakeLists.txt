cmake_minimum_required(VERSION 3.14)

project(Bookstore)

set(CMAKE_CXX_STANDARD 20)

include_directories(
        "${PROJECT_SOURCE_DIR}/include"
)

Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.8.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)

set(TEST_SOURCES
        tests/testMemoryRiver.cpp
)
add_executable(unit_tests ${TEST_SOURCES})
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
include(CTest)
include(Catch)
catch_discover_tests(unit_tests)


add_executable(t1 src/test.cpp)