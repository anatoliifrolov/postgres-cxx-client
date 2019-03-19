add_executable(${PROJECT_NAME}Example
        src/101_GetStarted.cpp
        src/102_GetStartedPool.cpp
        src/103_Headers.cpp
        src/104_Config.cpp
        src/105_Errors.cpp
        src/106_Exec.cpp
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
