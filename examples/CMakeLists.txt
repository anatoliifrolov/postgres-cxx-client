add_executable(${PROJECT_NAME}Example
        src/101_GetStarted.cpp
        src/102_GetStartedPool.cpp
        src/main.cpp
        )

find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME}Example
        PRIVATE
        ${PROJECT_NAME}
        Threads::Threads
        )

add_test(NAME ${PROJECT_NAME}Example
        COMMAND ${PROJECT_NAME}Example
        )
