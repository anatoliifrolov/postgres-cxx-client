#include <gtest/gtest.h>
#include <postgres/Command.h>
#include <postgres/Config.h>
#include <postgres/Connection.h>
#include <postgres/PreparedCommand.h>
#include <postgres/PrepareData.h>
#include <postgres/Receiver.h>
#include <postgres/Result.h>
#include "Connect.h"

namespace postgres {

TEST(ConnectionTest, Ping) {
    ASSERT_EQ(PQPING_OK, Connection::ping());
    ASSERT_EQ(PQPING_OK, Connection::ping(CONNECT_STR));
    ASSERT_EQ(PQPING_OK, Connection::ping(CONNECT_URI));

    ASSERT_EQ(PQPING_NO_RESPONSE, Connection::ping(Config::Builder{}.port(2345).build()));
    ASSERT_EQ(PQPING_NO_RESPONSE, Connection::ping("port=2345"));
    ASSERT_EQ(PQPING_NO_RESPONSE, Connection::ping("postgresql://:2345"));

    ASSERT_EQ(PQPING_NO_ATTEMPT, Connection::ping(Config::Builder{}.set("k", "v").build()));
    ASSERT_EQ(PQPING_NO_ATTEMPT, Connection::ping("k=v"));
    ASSERT_EQ(PQPING_NO_ATTEMPT, Connection::ping("postgresql://?k=v"));
}

TEST(ConnectionTest, Connect) {
    Connection conn{};
    ASSERT_TRUE(conn.isOk());
    ASSERT_TRUE(conn.message().empty());
    ASSERT_TRUE(conn.reset());
}

TEST(ConnectionTest, ConnectBad) {
    Connection conn{Config::Builder{}.port(2345).build()};
    ASSERT_FALSE(conn.isOk());
    ASSERT_FALSE(conn.message().empty());
    ASSERT_FALSE(conn.reset());
}

TEST(ConnectionTest, ConnectStr) {
    Connection conn{CONNECT_STR};
    ASSERT_TRUE(conn.isOk());
    ASSERT_TRUE(conn.message().empty());
    ASSERT_TRUE(conn.reset());
}

TEST(ConnectionTest, ConnectStrBad) {
    Connection conn{"port=2345"};
    ASSERT_FALSE(conn.isOk());
    ASSERT_FALSE(conn.message().empty());
    ASSERT_FALSE(conn.reset());
}

TEST(ConnectionTest, ConnectUri) {
    Connection conn{CONNECT_URI};
    ASSERT_TRUE(conn.isOk());
    ASSERT_TRUE(conn.message().empty());
    ASSERT_TRUE(conn.reset());
}

TEST(ConnectionTest, ConnectUriBad) {
    Connection conn{"postgresql://:2345"};
    ASSERT_FALSE(conn.isOk());
    ASSERT_FALSE(conn.message().empty());
    ASSERT_FALSE(conn.reset());
}

TEST(ConnectionTest, Exec) {
    Connection conn{};
    ASSERT_TRUE(conn.exec("SELECT 1").isOk());
    ASSERT_FALSE(conn.exec("SELECT 1; SELECT 2").isOk());
    ASSERT_FALSE(conn.exec("BAD").isOk());
}

TEST(ConnectionTest, ExecRaw) {
    Connection conn{};
    ASSERT_TRUE(conn.execRaw("SELECT 1").isOk());
    ASSERT_TRUE(conn.execRaw("SELECT 1; SELECT 2").isOk());
    ASSERT_FALSE(conn.execRaw("BAD").isOk());
}

TEST(ConnectionTest, Prepare) {
    Connection conn{};
    ASSERT_TRUE(conn.exec(PrepareData{"select1", "SELECT 1"}).isOk());
    ASSERT_TRUE(conn.exec(PreparedCommand{"select1"}).isOk());
    ASSERT_FALSE(conn.exec(PrepareData{"bad", "BAD"}).isOk());
    ASSERT_FALSE(conn.exec(PreparedCommand{"bad"}).isOk());
}

TEST(ConnectionTest, ExecAsync) {
    Connection conn{};
    ASSERT_TRUE(conn.send("SELECT 1").receive().isOk());
    ASSERT_FALSE(conn.send("SELECT 1; SELECT 2").receive().isOk());
    ASSERT_FALSE(conn.send("BAD").receive().isOk());
}

TEST(ConnectionTest, ExecRawAsync) {
    Connection conn{};
    ASSERT_TRUE(conn.sendRaw("SELECT 1").consume().isOk());
    ASSERT_TRUE(conn.sendRaw("SELECT 1; SELECT 2").consume().isOk());
    ASSERT_FALSE(conn.sendRaw("BAD").consume().isOk());
}

TEST(ConnectionTest, PrepareAsync) {
    Connection conn{};
    ASSERT_TRUE(conn.send(PrepareData{"select1", "SELECT 1"}).receive().isOk());
    ASSERT_TRUE(conn.send(PreparedCommand{"select1"}).receive().isOk());
    ASSERT_FALSE(conn.send(PrepareData{"bad", "BAD"}).receive().isOk());
    ASSERT_FALSE(conn.send(PreparedCommand{"bad"}).receive().isOk());
}

TEST(ConnectionTest, RowByRow) {
    Connection conn{};
    ASSERT_TRUE(conn.iter("SELECT 1").receive().isOk());
    ASSERT_FALSE(conn.iter("SELECT 1; SELECT 2").receive().isOk());
    ASSERT_FALSE(conn.iter("BAD").receive().isOk());
}

TEST(ConnectionTest, PrepareRowByRow) {
    Connection conn{};
    ASSERT_TRUE(conn.exec(PrepareData{"select1", "SELECT 1"}).isOk());
    ASSERT_TRUE(conn.iter(PreparedCommand{"select1"}).receive().isOk());
    ASSERT_FALSE(conn.exec(PrepareData{"bad", "BAD"}).isOk());
    ASSERT_FALSE(conn.iter(PreparedCommand{"bad"}).receive().isOk());
}

TEST(ConnectionTest, Esc) {
    Connection conn{};
    ASSERT_EQ("'E''SCAPE_ME'", conn.esc("E'SCAPE_ME"));
    ASSERT_EQ("\"e'scapeMe\"", conn.escId("e'scapeMe"));
}

}  // namespace postgres
