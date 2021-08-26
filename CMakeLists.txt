cmake_minimum_required(VERSION 3.10)
# cmake_policy(SET CMP0074 NEW)

project(gomoku-battle)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
link_directories(
    ${PROJECT_SOURCE_DIR}/bin
)

if(WIN32 AND NOT MINGW)
	add_compile_options("/std:c++latest")
	add_compile_options("/Zc:__cplusplus") # bugs in vs for nlohmann json
	add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS) # cancel min max macro in vs17
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_DEBUG_POSTFIX "_d")
endif()
if(UNIX OR MINGW)
	set( CMAKE_CXX_FLAGS "-std=c++20")
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		add_compile_options("-g")
		add_definitions(-D__debug__)
	endif()
endif()

add_subdirectory(utils)
add_subdirectory(serialize)
add_subdirectory(svr)
add_subdirectory(gomoku-svr)
add_subdirectory(gomoku-cc)
add_subdirectory(gomoku-demo)
add_subdirectory(tests)
