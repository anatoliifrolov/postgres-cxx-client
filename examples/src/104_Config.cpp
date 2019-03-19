/// ### Configuring
///
/// Beign a wrapper around libpq, the library just gives you a convinient interface to its features.
/// So this documentation mostly desribes the interface and is not a comprehensive reference.
/// For further details see official libpq docs https://www.postgresql.org/docs/11/libpq-connect.html.
///
/// Postgres has default values for every of its configuration parameter.
/// For instance, username defaults to the operating system name of the user running the app,
/// and database name is the same as the user name.
/// You have several ways to override the defaults:
/// - environment variables;
/// - connect string;
/// - URL;
/// - configuration builder.
///
/// The following four examples essentially do the same thing:
/// ```
#include <postgres/Postgres.h>

using postgres::Config;
using postgres::Connection;

void configDefault() {
    Connection conn{};
    conn.check();
}

/// ```
/// Here we connect to a database using the default values and environment variables.
/// This is a good choise to pass sensitive information like passwords.
/// For example, the library is tested assuming that PGUSER, PGPASSWORD and PGDATABASE are set.
///
/// Another way is to use a connect string:
/// ```
void configConnectStr() {
    Connection conn{"user=cxx_client password=cxx_client dbname=cxx_client"};
    conn.check();
}

/// ```
/// ...or URL:
/// ```
void configUrl() {
    Connection conn{"postgresql://cxx_client:cxx_client@/cxx_client"};
    conn.check();
}

/// ```
/// And the last one option is to use a configuration builder:
/// ```
void configBuilder() {
    Connection conn{Config::Builder{}.user("cxx_client")
                                     .password("cxx_client")
                                     .dbname("cxx_client")
                                     .build()};
    conn.check();
}
/// ```
/// The class ```Config::Builder``` provides setter methods for all parameters
/// available (and not deprecated) at the moment of writing.
/// Method names are in *snake_case* to exactly mirror corresponding parameter names.
/// An example setting some random parameters:
/// ```
#include <chrono>

using namespace std::chrono_literals;
using postgres::SslMode;

void configBuilderExtra() {
    Connection conn{Config::Builder{}.application_name("APP")
                                     .keepalives(true)
                                     .keepalives_count(2)
                                     .keepalives_idle(3min)
                                     .sslmode(SslMode::DISABLE)
                                     .build()};
    conn.check();
}

/// ```
/// Also there are some general purpose setters.
/// The following example manually sets the same parameters as the previous one:
/// ```
void configBuilderManual() {
    Connection conn{Config::Builder{}.set("application_name", "APP")
                                     .enable("keepalives", true)
                                     .setNumber("keepalives_count", 2)
                                     .setInterval("keepalives_idle", 3min)
                                     .set("sslmode", "disable")
                                     .build()};
    conn.check();
}
/// ```
