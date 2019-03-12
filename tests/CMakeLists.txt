project(postgres-cxx-client-test)

add_executable(${PROJECT_NAME}
        src/ChannelFake.cpp
        src/ChannelMock.cpp
        src/ChannelTest.cpp
        src/ClientTest.cpp
        src/CommandTest.cpp
        src/ConfigTest.cpp
        src/ConnectionTest.cpp
        src/ContextTest.cpp
        src/DispatcherTest.cpp
        src/FieldTest.cpp
        src/main.cpp
        src/ReceiverTest.cpp
        src/ResultTest.cpp
        src/RowTest.cpp
        src/Samples.cpp
        src/StatementTest.cpp
        src/TableTest.cpp
        src/TimeTest.cpp
        src/TransactionTest.cpp
        src/WorkerTest.cpp)

target_include_directories(${PROJECT_NAME}
        PRIVATE ../deps/googletest/googletest/include
        PRIVATE ../deps/googletest/googlemock/include)

target_link_libraries(${PROJECT_NAME}
        PRIVATE postgres-cxx-client
        PRIVATE gtest_main
        PRIVATE gmock_main)
