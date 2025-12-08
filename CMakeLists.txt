cmake_minimum_required(VERSION 3.14)

project(Bookstore)

set(CMAKE_CXX_STANDARD 20)

# solving the problem 2967
add_executable(code src/P2967.cpp)

include_directories(
        "${PROJECT_SOURCE_DIR}/include"
)
add_compile_options(-g)

Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.8.1 # or a later release
)
execute_process(
        COMMAND git ls-remote https://github.com/catchorg/Catch2.git HEAD
        RESULT_VARIABLE GIT_CONNECTION_STATUS
        OUTPUT_QUIET
        ERROR_QUIET
)

if (GIT_CONNECTION_STATUS EQUAL 0)
    FetchContent_MakeAvailable(Catch2)
    set(TEST_SOURCES
            tests/testMemoryRiver.cpp
            tests/testBlockList.cpp
            src/BooksManager.cpp
            tests/testUtils.cpp
            tests/testBooksManager.cpp
    )
    add_executable(unit_tests ${TEST_SOURCES})
    target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
    include(CTest)
    include(Catch)
    catch_discover_tests(unit_tests)
else ()
    message(STATUS "Cannot connect to Catch2 repository. Skipping FetchContent.")
endif ()
