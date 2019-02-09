project(postgres-cxx-client-example)

file(GLOB_RECURSE SRC src/*)
add_executable(postgres-cxx-client-example ${SRC})
target_link_libraries(
        postgres-cxx-client-example
        postgres-cxx-client
        pq
)
