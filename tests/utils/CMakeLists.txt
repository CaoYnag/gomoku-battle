cmake_minimum_required(VERSION 3.10)

project(test_utils)


set(SRC
    ${PROJECT_SOURCE_DIR}/test.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} utils)
