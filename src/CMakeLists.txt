add_executable(test test.cpp)
target_include_directories(test PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
)