#include <stdexcept>
#include <thread>
#include <set>
#include <gtest/gtest.h>
#include <postgres/Connection.h>
#include <postgres/Config.h>
#include <postgres/Command.h>
#include <postgres/PreparedCommand.h>
#include "Migration.h"

namespace postgres {

struct TestConnection : Migration, testing::Test {};

TEST_F(TestConnection, Ping) {
    ASSERT_TRUE(Connection::ping(Config{}));
    ASSERT_FALSE(Connection::ping(Config::init()
        .port(1234)
        .build()));
}

TEST_F(TestConnection, Bad) {
    Connection conn{postgres::Config::init()
        .dbname("BADDB")
        .user("BADUSER")
        .password("BADPASSW")
        .build()};
    ASSERT_FALSE(conn.isOk());
}

TEST_F(TestConnection, Good) {
    ASSERT_TRUE(conn_->isOk());
    conn_->reset();
    ASSERT_TRUE(conn_->isOk());
}

TEST_F(TestConnection, Esc) {
    ASSERT_EQ("'H''UERAGA'", conn_->esc("H'UERAGA"));
    ASSERT_EQ("\"h'ueRaga\"", conn_->escId("h'ueRaga"));
}

TEST_F(TestConnection, Raw) {
    auto status = conn_->executeRaw("SELECT 1; SELECT 2, 3;");
    ASSERT_TRUE(status.isOk());
    ASSERT_EQ(1, status.size());
    ASSERT_FALSE(status.empty());
}

TEST_F(TestConnection, Async) {
    ASSERT_TRUE(conn_->send(PrepareData{
        "prepared_insert",
        "INSERT INTO test(flag) VALUES($1)"}));
    ASSERT_TRUE(conn_->nextResult());
    ASSERT_TRUE(conn_->nextResult().isDone());
    ASSERT_TRUE(conn_->send("SELECT 1::INTEGER"));
    ASSERT_EQ(1, (int) conn_->nextResult()[0][0]);
    ASSERT_TRUE(conn_->nextResult().isDone());
    ASSERT_TRUE(conn_->send(PreparedCommand{"prepared_insert", true}));
    ASSERT_EQ(1, conn_->nextResult().affected());
    ASSERT_TRUE(conn_->nextResult().isDone());
}

TEST_F(TestConnection, Busy) {
    ASSERT_TRUE(conn_->send("SELECT 1::INTEGER, 2::INTEGER, 3::INTEGER"));
    while (conn_->isBusy()) {
        std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
    auto res = conn_->nextResult();
    ASSERT_EQ(1, (int) res[0][0]);
    ASSERT_EQ(2, (int) res[0][1]);
    ASSERT_EQ(3, (int) res[0][2]);
    ASSERT_TRUE(conn_->nextResult().isDone());
}

TEST_F(TestConnection, RowByRow) {
    ASSERT_TRUE(conn_->execute("INSERT INTO test(int4) VALUES(1), (2), (3)"));
    ASSERT_TRUE(conn_->send("SELECT int4 FROM test", true));
    std::set<int> data{};
    for (auto res = conn_->nextResult(); !res.isDone(); res = conn_->nextResult()) {
        if (!res.empty()) {
            ASSERT_EQ(1, res.size());
            data.insert((int) res[0][0]);
        }
    }
    ASSERT_EQ((std::set<int>{1, 2, 3}), data);
}

}  // namespace postgres
