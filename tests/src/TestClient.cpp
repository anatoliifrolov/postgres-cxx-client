#include <gtest/gtest.h>
#include <postgres/Client.h>
#include <postgres/Command.h>
#include <postgres/Config.h>
#include <postgres/PreparedCommand.h>
#include <postgres/PrepareData.h>
#include <postgres/Receiver.h>
#include <postgres/Result.h>

namespace postgres {

static constexpr auto CONNECT_STR = "user=cxx_client password=cxx_client dbname=cxx_client";
static constexpr auto CONNECT_URI = "postgresql://cxx_client:cxx_client@/cxx_client";

TEST(TestClient, Ping) {
    ASSERT_EQ(PQPING_OK, Client::ping());
    ASSERT_EQ(PQPING_OK, Client::ping(CONNECT_STR));
    ASSERT_EQ(PQPING_OK, Client::ping(CONNECT_URI));

    ASSERT_EQ(PQPING_NO_RESPONSE, Client::ping(Config::Builder{}.port(2345).build()));
    ASSERT_EQ(PQPING_NO_RESPONSE, Client::ping("port=2345"));
    ASSERT_EQ(PQPING_NO_RESPONSE, Client::ping("postgresql://:2345"));

    ASSERT_EQ(PQPING_NO_ATTEMPT, Client::ping(Config::Builder{}.set("k", "v").build()));
    ASSERT_EQ(PQPING_NO_ATTEMPT, Client::ping("k=v"));
    ASSERT_EQ(PQPING_NO_ATTEMPT, Client::ping("postgresql://?k=v"));
}

TEST(TestClient, Connect) {
    Client cl{};
    ASSERT_TRUE(cl.isOk());
    ASSERT_TRUE(cl.message().empty());
    ASSERT_TRUE(cl.reset());
}

TEST(TestClient, ConnectBad) {
    Client cl{Config::Builder{}.port(2345).build()};
    ASSERT_FALSE(cl.isOk());
    ASSERT_FALSE(cl.message().empty());
    ASSERT_FALSE(cl.reset());
}

TEST(TestClient, ConnectStr) {
    Client cl{CONNECT_STR};
    ASSERT_TRUE(cl.isOk());
    ASSERT_TRUE(cl.message().empty());
    ASSERT_TRUE(cl.reset());
}

TEST(TestClient, ConnectStrBad) {
    Client cl{"port=2345"};
    ASSERT_FALSE(cl.isOk());
    ASSERT_FALSE(cl.message().empty());
    ASSERT_FALSE(cl.reset());
}

TEST(TestClient, ConnectUri) {
    Client cl{CONNECT_URI};
    ASSERT_TRUE(cl.isOk());
    ASSERT_TRUE(cl.message().empty());
    ASSERT_TRUE(cl.reset());
}

TEST(TestClient, ConnectUriBad) {
    Client cl{"postgresql://:2345"};
    ASSERT_FALSE(cl.isOk());
    ASSERT_FALSE(cl.message().empty());
    ASSERT_FALSE(cl.reset());
}

TEST(TestClient, Exec) {
    Client cl{};
    ASSERT_TRUE(cl.exec("SELECT 1").isOk());
    ASSERT_FALSE(cl.exec("SELECT 1; SELECT 2").isOk());
    ASSERT_FALSE(cl.exec("BAD").isOk());
}

TEST(TestClient, ExecRaw) {
    Client cl{};
    ASSERT_TRUE(cl.execRaw("SELECT 1").isOk());
    ASSERT_TRUE(cl.execRaw("SELECT 1; SELECT 2").isOk());
    ASSERT_FALSE(cl.execRaw("BAD").isOk());
}

TEST(TestClient, Prepare) {
    Client cl{};
    ASSERT_TRUE(cl.exec(PrepareData{"select1", "SELECT 1"}).isOk());
    ASSERT_TRUE(cl.exec(PreparedCommand{"select1"}).isOk());
    ASSERT_FALSE(cl.exec(PrepareData{"bad", "BAD"}).isOk());
    ASSERT_FALSE(cl.exec(PreparedCommand{"bad"}).isOk());
}

TEST(TestClient, ExecAsync) {
    Client cl{};
    ASSERT_TRUE(cl.send("SELECT 1").receive().isOk());
    ASSERT_FALSE(cl.send("SELECT 1; SELECT 2").receive().isOk());
    ASSERT_FALSE(cl.send("BAD").receive().isOk());
}

TEST(TestClient, ExecRawAsync) {
    Client cl{};
    ASSERT_TRUE(cl.sendRaw("SELECT 1").consume().isOk());
    ASSERT_TRUE(cl.sendRaw("SELECT 1; SELECT 2").consume().isOk());
    ASSERT_FALSE(cl.sendRaw("BAD").consume().isOk());
}

TEST(TestClient, PrepareAsync) {
    Client cl{};
    ASSERT_TRUE(cl.send(PrepareData{"select1", "SELECT 1"}).receive().isOk());
    ASSERT_TRUE(cl.send(PreparedCommand{"select1"}).receive().isOk());
    ASSERT_FALSE(cl.send(PrepareData{"bad", "BAD"}).receive().isOk());
    ASSERT_FALSE(cl.send(PreparedCommand{"bad"}).receive().isOk());
}

TEST(TestClient, RowByRow) {
    Client cl{};
    ASSERT_TRUE(cl.iter("SELECT 1").receive().isOk());
    ASSERT_FALSE(cl.iter("SELECT 1; SELECT 2").receive().isOk());
    ASSERT_FALSE(cl.iter("BAD").receive().isOk());
}

TEST(TestClient, PrepareRowByRow) {
    Client cl{};
    ASSERT_TRUE(cl.exec(PrepareData{"select1", "SELECT 1"}).isOk());
    ASSERT_TRUE(cl.iter(PreparedCommand{"select1"}).receive().isOk());
    ASSERT_FALSE(cl.exec(PrepareData{"bad", "BAD"}).isOk());
    ASSERT_FALSE(cl.iter(PreparedCommand{"bad"}).receive().isOk());
}

TEST(TestClient, Esc) {
    Client cl{};
    ASSERT_EQ("'E''SCAPE_ME'", cl.esc("E'SCAPE_ME"));
    ASSERT_EQ("\"e'scapeMe\"", cl.escId("e'scapeMe"));
}

}  // namespace postgres
