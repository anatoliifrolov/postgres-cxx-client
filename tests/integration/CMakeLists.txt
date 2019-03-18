cmake_minimum_required(VERSION 3.8)

project(PostgresCxxClientIntegration)
add_executable(${PROJECT_NAME} main.cpp)

find_package(PostgresCxxClient)
target_link_libraries(${PROJECT_NAME} PostgresCxxClient::PostgresCxxClient)

enable_testing()
add_test(NAME ${PROJECT_NAME}
        COMMAND ${PROJECT_NAME}
        )
