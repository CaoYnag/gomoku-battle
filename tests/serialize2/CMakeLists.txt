cmake_minimum_required(VERSION 3.10)

project(test_serialize2)


set(SRC
    ${PROJECT_SOURCE_DIR}/test.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
find_library(RTTR rttr_core)
target_link_libraries(${PROJECT_NAME} serialize ${RTTR})
