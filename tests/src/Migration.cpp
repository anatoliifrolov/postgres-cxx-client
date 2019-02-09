#include "Migration.h"

namespace postgres {

postgres::Config makeConfig() {
    auto config = postgres::Config{};
    config.set("application_name", "postgres-test");
    return config;
}

postgres::Config getConfig() {
    static const auto config = makeConfig();
    return config;
}

Migration::Migration() : client_{getConfig()}, conn_{&client_.connection()} {
    client_.execute(
        "DROP TABLE IF EXISTS test",
        R"(
            CREATE TABLE test(
                int2 SMALLINT,
                int4 INTEGER,
                int8 BIGINT,
                float4 REAL,
                float8 DOUBLE PRECISION,
                flag BOOLEAN,
                info TEXT,
                time TIMESTAMP))");
}

}  // namespace postgres
