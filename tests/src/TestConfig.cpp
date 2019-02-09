#include <gtest/gtest.h>
#include <postgres/Config.h>

namespace postgres {

TEST(TestConfig, Values) {
    auto const cfg = Config::init()
        .dbname("NAME")
        .user("USER")
        .password("PASS")
        .host("HOST")
        .port(1234)
        .set("connect_timeout", 56)
        .build();
    ASSERT_STREQ("NAME", cfg.get("dbname"));
    ASSERT_STREQ("USER", cfg.get("user"));
    ASSERT_STREQ("PASS", cfg.get("password"));
    ASSERT_STREQ("HOST", cfg.get("host"));
    ASSERT_STREQ("1234", cfg.get("port"));
    ASSERT_STREQ("56", cfg.get("connect_timeout"));
    ASSERT_EQ(nullptr, cfg.get("bad_key"));
}

}  // namespace postgres
