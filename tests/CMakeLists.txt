project(postgres-cxx-client-test)

file(GLOB_RECURSE TEST_SRC src/*)
add_executable(postgres-cxx-client-test ${TEST_SRC})
target_link_libraries(
        postgres-cxx-client-test
        postgres-cxx-client
        gtest_main
        gmock_main
)
