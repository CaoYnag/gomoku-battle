cmake_minimum_required(VERSION 3.10)
project(svr)

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
	${PROJECT_SOURCE_DIR}/player.cpp
    ${PROJECT_SOURCE_DIR}/room.cpp
    ${PROJECT_SOURCE_DIR}/game.cpp
    ${PROJECT_SOURCE_DIR}/match.cpp
)

add_library(svr ${SRC})
target_link_libraries(svr utils)
if(WIN32)
endif()
if(UNIX)
  target_link_libraries(svr pthread)
endif()
