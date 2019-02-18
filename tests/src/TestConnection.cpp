#include <gtest/gtest.h>
#include <postgres/Command.h>
#include <postgres/Config.h>
#include <postgres/Connection.h>
#include <postgres/PrepareData.h>
#include <postgres/Result.h>

namespace postgres {

static constexpr auto CONNECT_STR = "user=cxx_client password=cxx_client dbname=cxx_client";
static constexpr auto CONNECT_URI = "postgresql://cxx_client:cxx_client@/cxx_client";

TEST(TestConnection, Ping) {
    ASSERT_EQ(PQPING_OK, Connection::ping(Config::build()));
    ASSERT_EQ(PQPING_NO_RESPONSE, Connection::ping(Config::Builder{}.port(2345).build()));
    ASSERT_EQ(PQPING_NO_ATTEMPT, Connection::ping(Config::Builder{}.set("k", "v").build()));
}

TEST(TestConnection, PingStr) {
    ASSERT_EQ(PQPING_OK, Connection::ping(CONNECT_STR));
    ASSERT_EQ(PQPING_NO_RESPONSE, Connection::ping("port=2345"));
    ASSERT_EQ(PQPING_NO_ATTEMPT, Connection::ping("k=v"));
}

TEST(TestConnection, PingUri) {
    ASSERT_EQ(PQPING_OK, Connection::ping(CONNECT_URI));
    ASSERT_EQ(PQPING_NO_RESPONSE, Connection::ping("postgresql://:2345"));
    ASSERT_EQ(PQPING_NO_ATTEMPT, Connection::ping("postgresql://?k=v"));
}

TEST(TestConnection, Connect) {
    Connection conn{Config::build()};
    ASSERT_TRUE(conn.isOk());
    ASSERT_TRUE(conn.error().empty());
    ASSERT_NE(nullptr, conn.native());
}

TEST(TestConnection, ConnectBad) {
    Connection conn{Config::Builder{}.port(2345).build()};
    ASSERT_FALSE(conn.isOk());
    ASSERT_FALSE(conn.error().empty());
    ASSERT_NE(nullptr, conn.native());
}

TEST(TestConnection, ConnectStr) {
    Connection conn{CONNECT_STR};
    ASSERT_TRUE(conn.isOk());
    ASSERT_TRUE(conn.error().empty());
    ASSERT_NE(nullptr, conn.native());
}

TEST(TestConnection, ConnectStrBad) {
    Connection conn{"port=2345"};
    ASSERT_FALSE(conn.isOk());
    ASSERT_FALSE(conn.error().empty());
    ASSERT_NE(nullptr, conn.native());
}

TEST(TestConnection, ConnectUri) {
    Connection conn{CONNECT_URI};
    ASSERT_TRUE(conn.isOk());
    ASSERT_TRUE(conn.error().empty());
    ASSERT_NE(nullptr, conn.native());
}

TEST(TestConnection, ConnectUriBad) {
    Connection conn{"postgresql://:2345"};
    ASSERT_FALSE(conn.isOk());
    ASSERT_FALSE(conn.error().empty());
    ASSERT_NE(nullptr, conn.native());
}

TEST(TestConnection, Reset) {
    Connection conn{Config::build()};
    ASSERT_TRUE(conn.reset());
    ASSERT_TRUE(conn.isOk());
}

TEST(TestConnection, Exec) {
    Connection conn{Config::build()};
    ASSERT_TRUE(conn.exec(Command{"SELECT 1"}).isOk());
    ASSERT_FALSE(conn.exec(Command{"SELECT 1; SELECT 2"}).isOk());
    ASSERT_FALSE(conn.exec(Command{"BAD"}).isOk());
}

TEST(TestConnection, ExecRaw) {
    Connection conn{Config::build()};
    ASSERT_TRUE(conn.execRaw("SELECT 1").isOk());
    ASSERT_TRUE(conn.execRaw("SELECT 1; SELECT 2").isOk());
    ASSERT_FALSE(conn.execRaw("BAD").isOk());
}

TEST(TestConnection, Prepare) {
    Connection conn{Config::build()};
    ASSERT_TRUE(conn.prepare(PrepareData{"select1", "SELECT 1"}).isOk());
    ASSERT_TRUE(conn.execPrepared(Command{"select1"}).isOk());
    ASSERT_FALSE(conn.execPrepared(Command{"BAD"}).isOk());
}

TEST(TestConnection, Async) {
    Connection conn{Config::build()};
    ASSERT_TRUE(conn.execAsync(Command{"SELECT 1"}));

    auto res = conn.receive();
    ASSERT_TRUE(res.isOk());
    ASSERT_FALSE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = conn.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());
}

TEST(TestConnection, AsyncBad) {
    Connection conn{Config::build()};
    ASSERT_TRUE(conn.execAsync(Command{"BAD"}));

    auto res = conn.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = conn.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());
}

TEST(TestConnection, PrepareAsync) {
    Connection conn{Config::build()};
    ASSERT_TRUE(conn.prepareAsync(PrepareData{"select1", "SELECT 1"}));

    auto res = conn.receive();
    ASSERT_TRUE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = conn.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());

    ASSERT_TRUE(conn.execPreparedAsync(Command{"select1"}));

    res = conn.receive();
    ASSERT_TRUE(res.isOk());
    ASSERT_FALSE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = conn.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());
}

TEST(TestConnection, PrepareAsyncBad) {
    Connection conn{Config::build()};
    ASSERT_TRUE(conn.prepareAsync(PrepareData{"select1", "BAD"}));

    auto res = conn.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = conn.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());

    ASSERT_TRUE(conn.execPreparedAsync(Command{"select1"}));

    res = conn.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = conn.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());
}

//TEST_F(TestConnection, Esc) {
//    ASSERT_EQ("'H''UERAGA'", conn_->esc("H'UERAGA"));
//    ASSERT_EQ("\"h'ueRaga\"", conn_->escId("h'ueRaga"));
//}
//
//TEST_F(TestConnection, Raw) {
//    auto status = conn_->execRaw("SELECT 1; SELECT 2, 3;");
//    ASSERT_TRUE(status.isOk());
//    ASSERT_EQ(1, status.size());
//    ASSERT_FALSE(status.empty());
//}
//
//TEST_F(TestConnection, Async) {
//    ASSERT_TRUE(conn_->prepareAsync(PrepareData{"prepared_insert",
//                                                "INSERT INTO test(flag) VALUES($1)"}));
//    ASSERT_TRUE(conn_->receive());
//    ASSERT_TRUE(conn_->receive().isDone());
//    ASSERT_TRUE(conn_->send(Command{"SELECT 1::INTEGER"}));
//    ASSERT_EQ(1, (int) conn_->receive()[0][0]);
//    ASSERT_TRUE(conn_->receive().isDone());
//    ASSERT_TRUE(conn_->send(PreparedCommand{"prepared_insert", true}));
//    ASSERT_EQ(1, conn_->receive().affected());
//    ASSERT_TRUE(conn_->receive().isDone());
//}
//
//TEST_F(TestConnection, Busy) {
//    ASSERT_TRUE(conn_->send(Command{"SELECT 1::INTEGER, 2::INTEGER, 3::INTEGER"}));
//    while (conn_->isBusy()) {
//        std::this_thread::sleep_for(std::chrono::milliseconds{1});
//    }
//    auto res = conn_->receive();
//    ASSERT_EQ(1, (int) res[0][0]);
//    ASSERT_EQ(2, (int) res[0][1]);
//    ASSERT_EQ(3, (int) res[0][2]);
//    ASSERT_TRUE(conn_->receive().isDone());
//}
//
//TEST_F(TestConnection, RowByRow) {
//    ASSERT_TRUE(conn_->exec(Command{"INSERT INTO test(int4) VALUES(1), (2), (3)"}));
//    ASSERT_TRUE(conn_->send(Command{"SELECT int4 FROM test"}, AsyncMode::SINGLE_ROW));
//    std::set<int> data{};
//
//    for (auto res = conn_->receive(); !res.isDone(); res = conn_->receive()) {
//        if (!res.empty()) {
//            ASSERT_EQ(1, res.size());
//            data.insert((int) res[0][0]);
//        }
//    }
//    ASSERT_EQ((std::set<int>{1, 2, 3}), data);
//}

}  // namespace postgres
