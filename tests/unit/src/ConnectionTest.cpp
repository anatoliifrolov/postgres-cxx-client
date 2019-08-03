#include <gtest/gtest.h>
#include <postgres/Config.h>
#include <postgres/Connection.h>
#include <postgres/PreparedCommand.h>
#include <postgres/PrepareData.h>
#include <postgres/Receiver.h>
#include <postgres/Result.h>
#include "Samples.h"

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

TEST(ConnectionTest, ConnectStr) {
    Connection conn{CONNECT_STR};
    ASSERT_TRUE(conn.isOk());
    ASSERT_TRUE(conn.message().empty());
    ASSERT_TRUE(conn.reset());
}

TEST(ConnectionTest, ConnectUri) {
    Connection conn{CONNECT_URI};
    ASSERT_TRUE(conn.isOk());
    ASSERT_TRUE(conn.message().empty());
    ASSERT_TRUE(conn.reset());
}

TEST(ConnectionTest, ConnectBad) {
    ASSERT_THROW(Connection{Config::Builder{}.port(2345).build()}, RuntimeError);
    ASSERT_THROW(Connection{"port=2345"}, RuntimeError);
    ASSERT_THROW(Connection{"postgresql://:2345"}, RuntimeError);
}

TEST(ConnectionTest, Exec) {
    Connection conn{};
    ASSERT_TRUE(conn.exec("SELECT 1").isOk());
    ASSERT_THROW(conn.exec("SELECT 1; SELECT 2"), RuntimeError);
    ASSERT_THROW(conn.exec("BAD"), RuntimeError);
}

TEST(ConnectionTest, ExecRaw) {
    Connection conn{};
    ASSERT_TRUE(conn.execRaw("SELECT 1").isOk());
    ASSERT_TRUE(conn.execRaw("SELECT 1; SELECT 2").isOk());
    ASSERT_THROW(conn.execRaw("BAD"), RuntimeError);
}

TEST(ConnectionTest, Prepare) {
    Connection conn{};
    ASSERT_TRUE(conn.exec(PrepareData{"select1", "SELECT 1"}).isOk());
    ASSERT_TRUE(conn.exec(PreparedCommand{"select1"}).isOk());
    ASSERT_THROW(conn.exec(PrepareData{"bad", "BAD"}), RuntimeError);
    ASSERT_THROW(conn.exec(PreparedCommand{"bad"}), RuntimeError);
}

TEST(ConnectionTest, PrepareArgs) {
    Connection conn{};
    ASSERT_TRUE(conn.exec(PrepareData{"select1", "SELECT $1", {INT4OID}}).isOk());
    ASSERT_TRUE(conn.exec(PreparedCommand{"select1", 1}).isOk());
    ASSERT_TRUE(conn.exec(PrepareData{"bad", "SELECT $1"}).isOk());
    ASSERT_THROW(conn.exec(PreparedCommand{"bad", 2}), RuntimeError);
}

TEST(ConnectionTest, ExecAsync) {
    Connection conn{};
    ASSERT_TRUE(conn.send("SELECT 1").receive().isOk());
    ASSERT_THROW(conn.send("SELECT 1; SELECT 2").receive(), RuntimeError);
    ASSERT_THROW(conn.send("BAD").receive(), RuntimeError);
}

TEST(ConnectionTest, ExecRawAsync) {
    Connection conn{};
    ASSERT_TRUE(conn.sendRaw("SELECT 1").consume().isOk());
    ASSERT_TRUE(conn.sendRaw("SELECT 1; SELECT 2").consume().isOk());
    ASSERT_THROW(conn.sendRaw("BAD").consume(), RuntimeError);
}

TEST(ConnectionTest, PrepareAsync) {
    Connection conn{};
    ASSERT_TRUE(conn.send(PrepareData{"select1", "SELECT 1"}).receive().isOk());
    ASSERT_TRUE(conn.send(PreparedCommand{"select1"}).receive().isOk());
    ASSERT_THROW(conn.send(PrepareData{"bad", "BAD"}).receive(), RuntimeError);
    ASSERT_THROW(conn.send(PreparedCommand{"bad"}).receive(), RuntimeError);
}

TEST(ConnectionTest, RowByRow) {
    Connection conn{};
    ASSERT_TRUE(conn.iter("SELECT 1").receive().isOk());
    ASSERT_THROW(conn.iter("SELECT 1; SELECT 2").receive(), RuntimeError);
    ASSERT_THROW(conn.iter("BAD").receive(), RuntimeError);
}

TEST(ConnectionTest, PrepareRowByRow) {
    Connection conn{};
    ASSERT_TRUE(conn.exec(PrepareData{"select1", "SELECT 1"}).isOk());
    ASSERT_TRUE(conn.iter(PreparedCommand{"select1"}).receive().isOk());
    ASSERT_THROW(conn.exec(PrepareData{"bad", "BAD"}), RuntimeError);
    ASSERT_THROW(conn.iter(PreparedCommand{"bad"}).receive(), RuntimeError);
}

TEST(ConnectionTest, Esc) {
    Connection conn{};
    ASSERT_EQ("'E''SCAPE_ME'", conn.esc("E'SCAPE_ME"));
    ASSERT_EQ("\"e'scapeMe\"", conn.escId("e'scapeMe"));
}

}  // namespace postgres
