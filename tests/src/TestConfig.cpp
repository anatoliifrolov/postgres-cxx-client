#include <gtest/gtest.h>
#include <postgres/Config.h>

namespace postgres {

TEST(TestConfig, Default) {
    postgres::Config config{};
    const auto& keys = config.keywords();
    const auto& vals = config.values();
    ASSERT_STREQ(keys[0], "hostaddr");
    ASSERT_STREQ(vals[0], "127.0.0.1");
    ASSERT_STREQ(keys[1], "port");
    ASSERT_STREQ(vals[1], "5432");
    ASSERT_EQ(keys[2], nullptr);
    ASSERT_EQ(vals[2], nullptr);
}

TEST(TestConfig, Basic) {
    postgres::Config config{"NAME", "USER", "PASS", "HOST", 1234};
    config.set("connect_timeout", 56);
    const auto& keys = config.keywords();
    const auto& vals = config.values();
    ASSERT_STREQ(keys[0], "dbname");
    ASSERT_STREQ(vals[0], "NAME");
    ASSERT_STREQ("NAME", config.get("dbname"));
    ASSERT_STREQ(keys[1], "user");
    ASSERT_STREQ(vals[1], "USER");
    ASSERT_STREQ("USER", config.get("user"));
    ASSERT_STREQ(keys[2], "password");
    ASSERT_STREQ(vals[2], "PASS");
    ASSERT_STREQ("PASS", config.get("password"));
    ASSERT_STREQ(keys[3], "host");
    ASSERT_STREQ(vals[3], "HOST");
    ASSERT_STREQ("HOST", config.get("host"));
    ASSERT_STREQ(keys[4], "port");
    ASSERT_STREQ(vals[4], "1234");
    ASSERT_STREQ("1234", config.get("port"));
    ASSERT_STREQ(keys[5], "connect_timeout");
    ASSERT_STREQ(vals[5], "56");
    ASSERT_STREQ("56", config.get("connect_timeout"));
    ASSERT_EQ(keys[6], nullptr);
    ASSERT_EQ(vals[6], nullptr);
    ASSERT_EQ(nullptr, config.get("bad_key"));
}

}  // namespace postgres
