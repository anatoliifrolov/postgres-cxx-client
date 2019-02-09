#include <gtest/gtest.h>
#include <postgres/Config.h>

namespace postgres {

TEST(TestConfig, Values) {
    auto const cfg = Config::init().host("HOST")
                                   .hostaddr("ADDR")
                                   .port(1234)
                                   .user("USER")
                                   .password("PASS")
                                   .dbname("NAME")
                                   .set("connect_timeout", 2345)
                                   .build();
    ASSERT_STREQ("HOST", cfg.get("host"));
    ASSERT_STREQ("ADDR", cfg.get("hostaddr"));
    ASSERT_STREQ("1234", cfg.get("port"));
    ASSERT_STREQ("USER", cfg.get("user"));
    ASSERT_STREQ("PASS", cfg.get("password"));
    ASSERT_STREQ("NAME", cfg.get("dbname"));
    ASSERT_STREQ("2345", cfg.get("connect_timeout"));
    ASSERT_EQ(nullptr, cfg.get("bad_key"));
}

}  // namespace postgres
