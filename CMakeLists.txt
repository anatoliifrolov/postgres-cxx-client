cmake_minimum_required(VERSION 3.5)
project(postgres-cxx-client VERSION 0.1.0)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
option(POSTGRES_CXX_BUILD_TESTS "Build tests" OFF)
option(POSTGRES_CXX_BUILD_EXAMPLES "Build examples" OFF)

find_path(
    LIBPQ_INCLUDES
    NAMES libpq-fe.h
    PATHS /usr/include/postgresql
)

include_directories(${LIBPQ_INCLUDES})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if (POSTGRES_CXX_BUILD_TESTS)
    include_directories(deps/googletest/googletest/include)
    include_directories(deps/googletest/googlemock/include)
    add_subdirectory(deps/googletest)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()

if (POSTGRES_CXX_BUILD_EXAMPLES)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif()

file(GLOB_RECURSE SRC src/* include/*)
add_library(postgres-cxx-client ${SRC})
