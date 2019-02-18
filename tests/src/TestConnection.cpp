#include <gtest/gtest.h>
#include <postgres/Command.h>
#include <postgres/Config.h>
#include <postgres/Connection.h>
#include <postgres/PrepareData.h>
#include <postgres/Receiver.h>
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
    ASSERT_TRUE(conn.message().empty());
    ASSERT_NE(nullptr, conn.native());
}

TEST(TestConnection, ConnectBad) {
    Connection conn{Config::Builder{}.port(2345).build()};
    ASSERT_FALSE(conn.isOk());
    ASSERT_FALSE(conn.message().empty());
    ASSERT_NE(nullptr, conn.native());
}

TEST(TestConnection, ConnectStr) {
    Connection conn{CONNECT_STR};
    ASSERT_TRUE(conn.isOk());
    ASSERT_TRUE(conn.message().empty());
    ASSERT_NE(nullptr, conn.native());
}

TEST(TestConnection, ConnectStrBad) {
    Connection conn{"port=2345"};
    ASSERT_FALSE(conn.isOk());
    ASSERT_FALSE(conn.message().empty());
    ASSERT_NE(nullptr, conn.native());
}

TEST(TestConnection, ConnectUri) {
    Connection conn{CONNECT_URI};
    ASSERT_TRUE(conn.isOk());
    ASSERT_TRUE(conn.message().empty());
    ASSERT_NE(nullptr, conn.native());
}

TEST(TestConnection, ConnectUriBad) {
    Connection conn{"postgresql://:2345"};
    ASSERT_FALSE(conn.isOk());
    ASSERT_FALSE(conn.message().empty());
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
    ASSERT_TRUE(conn.send(Command{"SELECT 1"}).receive().isOk());
    ASSERT_FALSE(conn.exec(Command{"SELECT 1; SELECT 2"}).isOk());
    ASSERT_FALSE(conn.send(Command{"SELECT 1; SELECT 2"}).receive().isOk());
    ASSERT_FALSE(conn.exec(Command{"BAD"}).isOk());
    ASSERT_FALSE(conn.send(Command{"BAD"}).receive().isOk());

    ASSERT_TRUE(conn.execRaw("SELECT 1").isOk());
    ASSERT_TRUE(conn.sendRaw("SELECT 1").receive().isOk());
    ASSERT_TRUE(conn.execRaw("SELECT 1; SELECT 2").isOk());
    ASSERT_TRUE(conn.sendRaw("SELECT 1; SELECT 2").receive().isOk());
    ASSERT_FALSE(conn.execRaw("BAD").isOk());
    ASSERT_FALSE(conn.sendRaw("BAD").receive().isOk());

    ASSERT_TRUE(conn.prepare(PrepareData{"select1", "SELECT 1"}).isOk());
    ASSERT_TRUE(conn.prepareAsync(PrepareData{"select2", "SELECT 2"}).receive().isOk());
    ASSERT_TRUE(conn.execPrepared(Command{"select1"}).isOk());
    ASSERT_TRUE(conn.sendPrepared(Command{"select2"}).receive().isOk());
    ASSERT_FALSE(conn.execPrepared(Command{"BAD"}).isOk());
    ASSERT_FALSE(conn.sendPrepared(Command{"BAD"}).receive().isOk());
}

TEST(TestConnection, Broken) {
    Connection conn{Config::Builder{}.port(2345).build()};
    ASSERT_FALSE(conn.exec(Command{"SELECT 1"}).isOk());
    ASSERT_FALSE(conn.send(Command{"SELECT 1"}).isOk());
    ASSERT_FALSE(conn.execRaw("SELECT 1").isOk());
    ASSERT_FALSE(conn.sendRaw("SELECT 1").isOk());
    ASSERT_FALSE(conn.prepare(PrepareData{"select1", "SELECT 1"}).isOk());
    ASSERT_FALSE(conn.execPrepared(Command{"select1"}).isOk());
    ASSERT_FALSE(conn.prepareAsync(PrepareData{"select1", "SELECT 1"}).isOk());
    ASSERT_FALSE(conn.sendPrepared(Command{"select1"}).isOk());
}

TEST(TestConnection, Esc) {
    Connection conn{Config::build()};
    ASSERT_EQ("'E''SCAPE_ME'", conn.esc("E'SCAPE_ME"));
    ASSERT_EQ("\"e'scapeMe\"", conn.escId("e'scapeMe"));
}

}  // namespace postgres
