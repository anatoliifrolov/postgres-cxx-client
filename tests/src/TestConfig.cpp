#include <map>
#include <string>
#include <gtest/gtest.h>
#include <postgres/Config.h>

using namespace std::chrono_literals;

namespace postgres {

TEST(TestConfig, Values) {
    auto const c = Config::Builder{}.application_name("APP")
                                    .client_encoding("ENC")
                                    .connect_timeout(1min)
                                    .dbname("DB")
                                    .fallback_application_name("APP2")
                                    .gsslib("GSS")
                                    .host("HOST")
                                    .hostaddr("ADDR")
                                    .keepalives(true)
                                    .keepalives_count(2)
                                    .keepalives_idle(3min)
                                    .keepalives_interval(4min)
                                    .krbsrvname("KRBS")
                                    .options("OPTS")
                                    .passfile("PASSF")
                                    .password("PASSW")
                                    .port(5)
                                    .requirepeer("PEER")
                                    .requiressl(true)
                                    .service("SVC")
                                    .sslcert("CERT")
                                    .sslcompression(true)
                                    .sslcrl("CRL")
                                    .sslkey("KEY")
                                    .sslrootcert("ROOT")
                                    .user("USER")
                                    .build();
    auto const k = c.keys();
    auto const v = c.values();
    auto       i = 0;
    ASSERT_STREQ(k[i], "application_name");
    ASSERT_STREQ(v[i], "APP");
    ++i;
    ASSERT_STREQ(k[i], "client_encoding");
    ASSERT_STREQ(v[i], "ENC");
    ++i;
    ASSERT_STREQ(k[i], "connect_timeout");
    ASSERT_STREQ(v[i], "60");
    ++i;
    ASSERT_STREQ(k[i], "dbname");
    ASSERT_STREQ(v[i], "DB");
    ++i;
    ASSERT_STREQ(k[i], "fallback_application_name");
    ASSERT_STREQ(v[i], "APP2");
    ++i;
    ASSERT_STREQ(k[i], "gsslib");
    ASSERT_STREQ(v[i], "GSS");
    ++i;
    ASSERT_STREQ(k[i], "host");
    ASSERT_STREQ(v[i], "HOST");
    ++i;
    ASSERT_STREQ(k[i], "hostaddr");
    ASSERT_STREQ(v[i], "ADDR");
    ++i;
    ASSERT_STREQ(k[i], "keepalives");
    ASSERT_STREQ(v[i], "1");
    ++i;
    ASSERT_STREQ(k[i], "keepalives_count");
    ASSERT_STREQ(v[i], "2");
    ++i;
    ASSERT_STREQ(k[i], "keepalives_idle");
    ASSERT_STREQ(v[i], "180");
    ++i;
    ASSERT_STREQ(k[i], "keepalives_interval");
    ASSERT_STREQ(v[i], "240");
    ++i;
    ASSERT_STREQ(k[i], "krbsrvname");
    ASSERT_STREQ(v[i], "KRBS");
    ++i;
    ASSERT_STREQ(k[i], "options");
    ASSERT_STREQ(v[i], "OPTS");
    ++i;
    ASSERT_STREQ(k[i], "passfile");
    ASSERT_STREQ(v[i], "PASSF");
    ++i;
    ASSERT_STREQ(k[i], "password");
    ASSERT_STREQ(v[i], "PASSW");
    ++i;
    ASSERT_STREQ(k[i], "port");
    ASSERT_STREQ(v[i], "5");
    ++i;
    ASSERT_STREQ(k[i], "requirepeer");
    ASSERT_STREQ(v[i], "PEER");
    ++i;
    ASSERT_STREQ(k[i], "requiressl");
    ASSERT_STREQ(v[i], "1");
    ++i;
    ASSERT_STREQ(k[i], "service");
    ASSERT_STREQ(v[i], "SVC");
    ++i;
    ASSERT_STREQ(k[i], "sslcert");
    ASSERT_STREQ(v[i], "CERT");
    ++i;
    ASSERT_STREQ(k[i], "sslcompression");
    ASSERT_STREQ(v[i], "1");
    ++i;
    ASSERT_STREQ(k[i], "sslcrl");
    ASSERT_STREQ(v[i], "CRL");
    ++i;
    ASSERT_STREQ(k[i], "sslkey");
    ASSERT_STREQ(v[i], "KEY");
    ++i;
    ASSERT_STREQ(k[i], "sslrootcert");
    ASSERT_STREQ(v[i], "ROOT");
    ++i;
    ASSERT_STREQ(k[i], "user");
    ASSERT_STREQ(v[i], "USER");
}

TEST(TestConfig, Manual) {
    auto const c = Config::Builder{}.set("application_name", "APP")
                                    .setInterval("connect_timeout", 1min)
                                    .enable("keepalives", true)
                                    .setNumber("keepalives_count", 2)
                                    .enable("sslcompression", false)
                                    .build();
    auto const k = c.keys();
    auto const v = c.values();
    auto       i = 0;
    ASSERT_STREQ(k[i], "application_name");
    ASSERT_STREQ(v[i], "APP");
    ++i;
    ASSERT_STREQ(k[i], "connect_timeout");
    ASSERT_STREQ(v[i], "60");
    ++i;
    ASSERT_STREQ(k[i], "keepalives");
    ASSERT_STREQ(v[i], "1");
    ++i;
    ASSERT_STREQ(k[i], "keepalives_count");
    ASSERT_STREQ(v[i], "2");
    ++i;
    ASSERT_STREQ(k[i], "sslcompression");
    ASSERT_STREQ(v[i], "0");
}

TEST(TestConfig, SslMode) {
    std::map<SslMode, std::string> const samples{{SslMode::ALLOW,       "allow"},
                                                 {SslMode::DISABLE,     "disable"},
                                                 {SslMode::PREFER,      "prefer"},
                                                 {SslMode::REQUIRE,     "require"},
                                                 {SslMode::VERIFY_CA,   "verify-ca"},
                                                 {SslMode::VERIFY_FULL, "verify-full"},
                                                 {SslMode::DEFAULT,     "prefer"}};

    for (auto const& sample : samples) {
        auto const c = Config::Builder{}.sslmode(sample.first).build();
        ASSERT_EQ(sample.second, c.values()[0]);
    }
}

TEST(TestConfig, TargetSession) {
    std::map<TargetSession, std::string> const samples{{TargetSession::ANY,        "any"},
                                                       {TargetSession::READ_WRITE, "read-write"},
                                                       {TargetSession::DEFAULT,    "any"}};

    for (auto const& sample : samples) {
        auto const c = Config::Builder{}.target_session_attrs(sample.first).build();
        ASSERT_EQ(sample.second, c.values()[0]);
    }
}

}  // namespace postgres
