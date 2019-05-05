cmake_minimum_required(VERSION 3.12)

# Project setup ---------------------------------
project(chord

	C
	CXX
)

## Platform setup
include(cmake/platform.cmake)

## Versioning
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0) # Not really using it

## Build type config
set(CMAKE_CXX_FLAGS_DEBUG			"-DSGL_BUILD_DEBUG=1 -DBUILD_DEBUG=1 -ggdb -O0")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO	"-DSGL_BUILD_DEVELOPMENT=1 -DBUILD_DEVELOPMENT=1 -ggdb -O1 -foptimize-sibling-calls")
set(CMAKE_CXX_FLAGS_RELEASE			"-DSGL_BUILD_RELEASE=1 -DBUILD_RELEASE=1 -Ofast")

# Compiler setup --------------------------------
## CXX preferences
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAG} -mavx2 -pthread -DWITH_THREADS_POOL=1")

# Code setup ------------------------------------
## Third party
add_subdirectory(vendor)

## Source code
add_subdirectory(src)