cmake_minimum_required(VERSION 3.14)

project(Bookstore)

set(CMAKE_CXX_STANDARD 20)

# solving the problem 2967
#add_executable(code src/P2967.cpp)

include_directories(
        "${PROJECT_SOURCE_DIR}/include"
)
#add_compile_options(-g)

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

set(MAIN_SOURCES
        src/BooksManager.cpp
        src/UsersManager.cpp
        src/Session.cpp
        src/Commands/UserCommands.cpp
        src/Commands/BookCommands.cpp
        src/Commands/LogCommands.cpp
        src/Parser/Parser.cpp
        src/Parser/UserParser.cpp
        src/Parser/BookParser.cpp
        src/Parser/LogParser.cpp
        src/LogManager.cpp
)
set(TEST_SOURCES
        tests/testMemoryRiver.cpp
        tests/testBlockList.cpp
        tests/testUtils.cpp
        tests/testBooksManager.cpp
        tests/testValidator.cpp
        tests/testUsersManager.cpp
        tests/testUserCommands.cpp
        tests/testBookCommands.cpp
)

add_executable(code ${MAIN_SOURCES} src/main.cpp)

if (GIT_CONNECTION_STATUS EQUAL 0)
    FetchContent_MakeAvailable(Catch2)
    add_executable(unit_tests ${TEST_SOURCES} ${MAIN_SOURCES})
    target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
    include(CTest)
    include(Catch)
    catch_discover_tests(unit_tests)
else ()
    message(STATUS "Cannot connect to Catch2 repository. Skipping FetchContent.")
endif ()
