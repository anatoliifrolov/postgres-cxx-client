cmake_minimum_required(VERSION 3.8)
project(postgres-cxx-client VERSION 0.1.0)

add_library(${PROJECT_NAME}
        src/Channel.cpp
        src/Client.cpp
        src/Command.cpp
        src/Config.cpp
        src/Connection.cpp
        src/Consumer.cpp
        src/Context.cpp
        src/Dispatcher.cpp
        src/Error.cpp
        src/Field.cpp
        src/IChannel.cpp
        src/Job.cpp
        src/PrepareData.cpp
        src/PreparedCommand.cpp
        src/Receiver.cpp
        src/Result.cpp
        src/Row.cpp
        src/Statement.cpp
        src/Status.cpp
        src/Time.cpp
        src/Transaction.cpp
        src/Visitable.cpp
        src/Visitors.cpp
        src/Worker.cpp
        )

target_compile_features(${PROJECT_NAME}
        PUBLIC
        cxx_std_17
        )

find_package(PostgreSQL REQUIRED)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        include
        ${PostgreSQL_INCLUDE_DIRS}
        )

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ${PostgreSQL_LIBRARIES}
        )

option(POSTGRES_CXX_BUILD_EXAMPLES "Build examples" OFF)
option(POSTGRES_CXX_BUILD_TESTS "Build tests" OFF)

if (POSTGRES_CXX_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (POSTGRES_CXX_BUILD_TESTS)
    add_subdirectory(deps/googletest)
    add_subdirectory(tests)
endif ()
