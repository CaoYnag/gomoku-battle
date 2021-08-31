cmake_minimum_required(VERSION 3.10)
project(icc) # interactive cui clients

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
    ${PROJECT_SOURCE_DIR}/cmds.cpp
    ${PROJECT_SOURCE_DIR}/cmd_io.cpp
	${PROJECT_SOURCE_DIR}/sync_io.cpp
    ${PROJECT_SOURCE_DIR}/csh.cpp
	${PROJECT_SOURCE_DIR}/user_agent.cpp
    ${PROJECT_SOURCE_DIR}/main.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} utils spes)
if(WIN32)
endif()
if(UNIX)
	target_link_libraries(${PROJECT_NAME} pthread)
endif()
