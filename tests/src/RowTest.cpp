#include <gtest/gtest.h>
#include <postgres/Connection.h>
#include <postgres/Error.h>
#include <postgres/Row.h>
#include <postgres/Visitable.h>

namespace postgres {

struct RowTestTable {
    int32_t x = 0;
    int32_t y = 0;

    POSTGRES_CXX_TABLE("row_test", x, y);
};

TEST(RowTest, Read) {
    auto const res = Connection{}.exec("SELECT 1::INT, 2::INT");
    auto       row = res[0];
    int32_t    x   = 0;
    int32_t    y   = 0;
    int32_t    z   = 0;
    row >> x >> y;
    ASSERT_EQ(1, x);
    ASSERT_EQ(2, y);
    ASSERT_THROW(row >> z, LogicError);
}

TEST(RowTest, Visit) {
    Connection   conn{};
    RowTestTable tbl{};
    conn.exec("SELECT 1::INT AS x, 2::INT AS y")[0] >> tbl;
    ASSERT_EQ(1, tbl.x);
    ASSERT_EQ(2, tbl.y);
    ASSERT_THROW(conn.exec("SELECT 1::INT")[0] >> tbl, LogicError);
}

TEST(RowTest, Index) {
    auto const res = Connection{}.exec("SELECT 1::INT, 2::INT");
    auto const row = res[0];
    ASSERT_EQ(2, row.size());
    ASSERT_EQ(1, row[0].as<int32_t>());
    ASSERT_EQ(2, row[1].as<int32_t>());
    ASSERT_THROW(row[2], LogicError);
    ASSERT_THROW(row[-1], LogicError);
}

TEST(RowTest, Name) {
    auto const res = Connection{}.exec("SELECT 1::INT AS x, 2::INT AS y");
    auto const row = res[0];
    ASSERT_EQ(2, row.size());
    ASSERT_EQ(1, row["x"].as<int32_t>());
    ASSERT_EQ(2, row["y"].as<int32_t>());
    ASSERT_THROW(row["bad"], LogicError);
}

}  // namespace postgres
