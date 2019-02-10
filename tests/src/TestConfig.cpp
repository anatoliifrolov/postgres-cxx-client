#include <gtest/gtest.h>
#include <postgres/Config.h>

namespace postgres {

TEST(TestConfig, Values) {
    auto const cfg = Config::Builder{}.host("HOST")
                                      .hostaddr("ADDR")
                                      .port(1234)
                                      .user("USER")
                                      .password("PASS")
                                      .dbname("NAME")
                                      .set("application_name", "APP")
                                      .set("connect_timeout", 2345)
                                      .build();
    auto const& keys = cfg.keys();
    auto const& vals = cfg.values();
    ASSERT_STREQ(keys[0], "application_name");
    ASSERT_STREQ(vals[0], "APP");
    ASSERT_STREQ(keys[1], "connect_timeout");
    ASSERT_STREQ(vals[1], "2345");
    ASSERT_STREQ(keys[2], "dbname");
    ASSERT_STREQ(vals[2], "NAME");
    ASSERT_STREQ(keys[3], "host");
    ASSERT_STREQ(vals[3], "HOST");
    ASSERT_STREQ(keys[4], "hostaddr");
    ASSERT_STREQ(vals[4], "ADDR");
    ASSERT_STREQ(keys[5], "password");
    ASSERT_STREQ(vals[5], "PASS");
    ASSERT_STREQ(keys[6], "port");
    ASSERT_STREQ(vals[6], "1234");
    ASSERT_STREQ(keys[7], "user");
    ASSERT_STREQ(vals[7], "USER");
    ASSERT_EQ(keys[8], nullptr);
    ASSERT_EQ(vals[8], nullptr);
}

}  // namespace postgres
