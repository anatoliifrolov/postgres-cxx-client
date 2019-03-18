add_executable(${PROJECT_NAME}Example
        src/001_GetStarted.cpp
        src/main.cpp
        )

target_link_libraries(${PROJECT_NAME}Example
        PRIVATE
        ${PROJECT_NAME}
        )

add_test(NAME ${PROJECT_NAME}Example
        COMMAND ${PROJECT_NAME}Example
        )
