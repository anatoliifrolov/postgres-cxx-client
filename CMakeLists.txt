cmake_minimum_required(VERSION 3.0)
project(postgres-cxx-client VERSION 0.1.0)

# todo: private flags
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall)

# todo: remove
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(POSTGRES_CXX_BUILD_TESTS "Build tests" OFF)
option(POSTGRES_CXX_BUILD_EXAMPLES "Build examples" OFF)

if (POSTGRES_CXX_BUILD_TESTS)
    include_directories(deps/googletest/googletest/include)
    include_directories(deps/googletest/googlemock/include)
    add_subdirectory(deps/googletest)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif ()

#if (POSTGRES_CXX_BUILD_EXAMPLES)
#    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
#endif ()

file(GLOB_RECURSE SRC src/*)
add_library(postgres-cxx-client ${SRC})
target_link_libraries(postgres-cxx-client PUBLIC pq)

find_path(LIBPQ_INCLUDES NAMES libpq-fe.h PATHS /usr/include/postgresql)
target_include_directories(postgres-cxx-client PUBLIC ${LIBPQ_INCLUDES})
target_include_directories(postgres-cxx-client PUBLIC include)
