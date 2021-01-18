cmake_minimum_required(VERSION 3.10)

project(test_msg_sock)

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

	find_package(Boost 1.67 COMPONENTS)
	if(Boost_FOUND)
		include_directories(${Boost_INCLUDE_DIRS})
	endif()
endif()

set(SRC_SVR
    ${PROJECT_SOURCE_DIR}/svr.cpp
)
set(SRC_CLI
    ${PROJECT_SOURCE_DIR}/cli.cpp
)


add_executable(${PROJECT_NAME}_svr ${SRC_SVR})
add_executable(${PROJECT_NAME}_cli ${SRC_CLI})
target_link_libraries(${PROJECT_NAME}_svr utils)
target_link_libraries(${PROJECT_NAME}_cli utils)
