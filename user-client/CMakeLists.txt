cmake_minimum_required(VERSION 3.10)
project(user-client)

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

set(SRC
    ${PROJECT_SOURCE_DIR}/client.cpp
)

add_executable(client ${SRC})
target_link_libraries(client boost_thread)
if(WIN32)
endif()
if(UNIX)
	target_link_libraries(client pthread)
endif()