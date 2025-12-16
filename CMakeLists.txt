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
        src/Commands/UserCommands.cpp
        src/Commands/BookCommands.cpp
        src/Commands/LogCommands.cpp
        src/Parser/Parser.cpp
        src/Parser/UserParser.cpp
        src/Parser/BookParser.cpp
        src/Parser/LogParser.cpp
        src/LogManager.cpp
        src/Utils.cpp
        src/Commands/CommandBase.cpp
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
    # unit test
    FetchContent_MakeAvailable(Catch2)
    add_executable(unit_tests ${TEST_SOURCES} ${MAIN_SOURCES})
    target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
    include(CTest)
    include(Catch)
    catch_discover_tests(unit_tests)

    # the server part
    # 1. Download ASIO for crow
    FetchContent_Declare(
            asio
            GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
            GIT_TAG asio-1-30-2 # 建议固定一个较新的版本 tag
    )

    FetchContent_MakeAvailable(asio)
    set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include)
    # 2. Download crow
    FetchContent_Declare(
            crow
            GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
            GIT_TAG master
    )
    FetchContent_MakeAvailable(crow)
    add_executable(server ${MAIN_SOURCES} src/server.cpp)
    target_include_directories(server PRIVATE ${crow_SOURCE_DIR}/include)


else ()
    message(STATUS "Cannot connect to Catch2 repository. Skipping FetchContent.")
endif ()
