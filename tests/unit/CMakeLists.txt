add_executable(PostgresCxxClientTest
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
        src/WorkerTest.cpp
        )

target_include_directories(PostgresCxxClientTest
        PRIVATE
        "${PROJECT_SOURCE_DIR}/deps/googletest/googletest/include"
        "${PROJECT_SOURCE_DIR}/deps/googletest/googlemock/include"
        )

target_link_libraries(PostgresCxxClientTest
        PRIVATE
        PostgresCxxClient
        gtest_main
        gmock_main
        )

add_test(NAME PostgresCxxClientTest
        COMMAND PostgresCxxClientTest
        )
