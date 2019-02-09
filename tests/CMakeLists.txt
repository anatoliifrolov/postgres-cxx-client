project(postgres-cxx-client-test)

file(GLOB_RECURSE SRC src/*)
add_executable(postgres-cxx-client-test ${SRC})
target_link_libraries(
        postgres-cxx-client-test
        postgres-cxx-client
        pq
        gtest_main
)
