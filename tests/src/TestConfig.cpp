#include <gtest/gtest.h>
#include <postgres/Config.h>

namespace postgres {

TEST(TestConfig, Basic) {
    auto const config = Config::init()
        .dbname("NAME")
        .user("USER")
        .password("PASS")
        .host("HOST")
        .port(1234)
        .set("connect_timeout", 56)
        .build();
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
