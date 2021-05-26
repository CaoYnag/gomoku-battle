cmake_minimum_required(VERSION 3.10)

project(utils)

set(SRC
    ${PROJECT_SOURCE_DIR}/def.cpp
    ${PROJECT_SOURCE_DIR}/utils.cpp
    ${PROJECT_SOURCE_DIR}/net_utils.cpp
    ${PROJECT_SOURCE_DIR}/config.cpp
    ${PROJECT_SOURCE_DIR}/msg.cpp
    ${PROJECT_SOURCE_DIR}/msg_sock.cpp
    ${PROJECT_SOURCE_DIR}/gomoku.cpp
)

if(WIN32)
	if($ENV{LIB_BASE})
	else()
		include_directories(
			$ENV{LIB_BASE}/include
		)
		link_directories(
			$ENV{LIB_BASE}/lib
		)
	endif()
endif()

set(LIB_NAME utils)
add_library(${LIB_NAME} STATIC ${SRC})
find_library(RTTR rttr_core)
target_link_libraries(${LIB_NAME} serialize ${RTTR})
