project(TRIANGLE)

cmake_minimum_required(VERSION 2.8.7)

set(CMAKE_CXX_FLAGS "-DTRILIBRARY -DLINUX -fPIC")

add_definitions(-DTRILIBRARY -DLINUX)

add_library(triangle triangle.h triangle.c)
set_target_properties(triangle PROPERTIES
	OUTPUT_NAME "triangle"
	FOLDER "TRIANGLE")

target_link_libraries(triangle lm)
