add_executable(${PROJECT_NAME}Test
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

target_include_directories(${PROJECT_NAME}Test
        PRIVATE
        "${PROJECT_SOURCE_DIR}/deps/googletest/googletest/include"
        "${PROJECT_SOURCE_DIR}/deps/googletest/googlemock/include"
        )

target_link_libraries(${PROJECT_NAME}Test
        PRIVATE
        ${PROJECT_NAME}
        gtest_main
        gmock_main
        )

add_test(NAME ${PROJECT_NAME}Test
        COMMAND ${PROJECT_NAME}Test
        )
