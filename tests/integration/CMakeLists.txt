cmake_minimum_required(VERSION 3.8)

project(PostgresCxxClientIntegration)
add_executable(PostgresCxxClientIntegration main.cpp)

find_package(PostgresCxxClient)
target_link_libraries(PostgresCxxClientIntegration PostgresCxxClient::PostgresCxxClient)

enable_testing()
add_test(NAME PostgresCxxClientIntegration
        COMMAND PostgresCxxClientIntegration
        )
