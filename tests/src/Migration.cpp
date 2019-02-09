#include "Migration.h"

namespace postgres {

Migration::Migration()
    : client_{}, conn_{&client_.connection()} {
    migrate();
    client_.execute("DELETE FROM test");
}

void Migration::migrate() {
    static auto is_done = false;
    if (is_done) {
        return;
    }

    client_.execute("DROP TABLE IF EXISTS test", R"(
            CREATE TABLE test(
                int2 SMALLINT,
                int4 INTEGER,
                int8 BIGINT,
                float4 REAL,
                float8 DOUBLE PRECISION,
                flag BOOLEAN,
                info TEXT,
                time TIMESTAMP))");
    is_done = true;
}

}  // namespace postgres
