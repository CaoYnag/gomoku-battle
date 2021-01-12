cmake_minimum_required(VERSION 3.10)

project(serialize)
set(SERIALIZE_VERSION_MAJOR 0)
set(SERIALIZE_VERSION_MINOR 0)
set(SERIALIZE_VERSION_PATCH 1)


include_directories(
    ${PROJECT_SOURCE_DIR}/
)

set(SRC
    ${PROJECT_SOURCE_DIR}/defines.cpp
    ${PROJECT_SOURCE_DIR}/serialize.cpp
    ${PROJECT_SOURCE_DIR}/deserialize.cpp
)

set(LIB_NAME serialize)
add_library(${LIB_NAME} ${LIB_TYPE} ${SRC})

find_library(RTTR rttr_core)
target_link_libraries(${LIB_NAME} ${RTTR})
