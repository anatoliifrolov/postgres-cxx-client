cmake_minimum_required(VERSION 3.0)
project(postgres-cxx-client VERSION 0.1.0)

option(POSTGRES_CXX_BUILD_EXAMPLES "Build examples" OFF)
option(POSTGRES_CXX_BUILD_TESTS "Build tests" OFF)

#if (POSTGRES_CXX_BUILD_EXAMPLES)
#    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
#endif ()

if (POSTGRES_CXX_BUILD_TESTS)
    # todo: find some better way
    add_subdirectory(tests)
    add_subdirectory(deps/googletest)
    target_include_directories(postgres-cxx-client-test PRIVATE deps/googletest/googletest/include)
    target_include_directories(postgres-cxx-client-test PRIVATE deps/googletest/googlemock/include)
endif ()

# todo: get rid of GLOB_RECURSE?
file(GLOB_RECURSE SRC src/*)
add_library(postgres-cxx-client ${SRC})
target_include_directories(postgres-cxx-client PUBLIC include)

# Resolve libpq dependency.
find_package(PostgreSQL REQUIRED)
target_include_directories(postgres-cxx-client PUBLIC ${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(postgres-cxx-client PUBLIC ${PostgreSQL_LIBRARIES})
