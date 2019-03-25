add_executable(PostgresCxxClientExample
        src/main.cpp
        src/usage.cpp
        )

find_package(Threads REQUIRED)

target_link_libraries(PostgresCxxClientExample
        PRIVATE
        PostgresCxxClient
        Threads::Threads
        )

add_test(NAME PostgresCxxClientExample
        COMMAND PostgresCxxClientExample
        )
