cmake_minimum_required(VERSION 3.5..3.14)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)


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
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

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
        src/Parser/FieldParser.cpp
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

    FetchContent_Declare(jwt-cpp
            GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
            GIT_TAG 08bcf77a687fb06e34138e9e9fa12a4ecbe12332 # v0.7.0 release
    )
    set(JWT_BUILD_EXAMPLES OFF CACHE BOOL "disable building examples" FORCE)
    FetchContent_MakeAvailable(jwt-cpp)

    add_executable(server ${MAIN_SOURCES} src/server.cpp)
    target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json)
    target_include_directories(server PRIVATE ${crow_SOURCE_DIR}/include)
    target_include_directories(server PRIVATE ${asio_SOURCE_DIR}/asio/include)
    target_link_libraries(server PRIVATE jwt-cpp::jwt-cpp)

    if(WIN32)
    target_link_libraries(server
            PRIVATE
            ws2_32
            mswsock
    )
    target_compile_definitions(server PRIVATE _WIN32_WINNT=0x0a00)
    endif()


else ()
    message(STATUS "Cannot connect to Catch2 repository. Skipping FetchContent.")
endif ()
