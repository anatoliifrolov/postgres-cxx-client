#include <chrono>
#include <string>
#include <vector>
#include <gtest/gtest.h>
#include <postgres/Statement.h>
#include <postgres/Visitable.h>

namespace postgres {

struct StatementTestTable {
    int a = 0;
    int b = 0;
    int c = 0;

    POSTGRES_CXX_TABLE("test", a, b, c)
};

struct StatementTestTable2 {
    bool                                  b;
    int16_t                               i2;
    int32_t                               i4;
    int64_t                               i8;
    uint16_t                              u2;
    uint32_t                              u4;
    uint64_t                              u8;
    float                                 f4;
    double                                f8;
    std::string                           s;
    std::chrono::system_clock::time_point t;

    POSTGRES_CXX_TABLE("test", b, i2, i4, i8, u2, u4, u8, f4, f8, s, t)
};

TEST(StatementTest, Create) {
    auto const query = "CREATE TABLE test ("
                       "b BOOL,"
                       "i2 SMALLINT,"
                       "i4 INT,"
                       "i8 BIGINT,"
                       "u2 SMALLSERIAL,"
                       "u4 SERIAL,"
                       "u8 BIGSERIAL,"
                       "f4 REAL,"
                       "f8 DOUBLE PRECISION,"
                       "s TEXT,"
                       "t TIMESTAMP)";
    ASSERT_EQ(query, Statement<StatementTestTable2>::create());
}

TEST(StatementTest, Insert) {
    auto const query = "INSERT INTO test (a,b,c) VALUES ($1,$2,$3)";
    ASSERT_EQ(query, Statement<StatementTestTable>::insert());
}

TEST(StatementTest, Select) {
    ASSERT_EQ("SELECT a,b,c FROM test", Statement<StatementTestTable>::select());
}

TEST(StatementTest, Update) {
    ASSERT_EQ("UPDATE test SET a=$1,b=$2,c=$3", Statement<StatementTestTable>::update());
}

TEST(StatementTest, Parts) {
    ASSERT_EQ("a,b,c", Statement<StatementTestTable>::fields());
    ASSERT_EQ("$1,$2,$3", Statement<StatementTestTable>::placeholders());
    ASSERT_EQ("$2,$3,$4", Statement<StatementTestTable>::placeholders(1));
    ASSERT_EQ("a=$1,b=$2,c=$3", Statement<StatementTestTable>::assignments());
    ASSERT_EQ("a=$2,b=$3,c=$4", Statement<StatementTestTable>::assignments(1));
}

TEST(StatementTest, Range) {
    auto const query = "INSERT INTO test (a,b,c) VALUES ($1,$2,$3),($4,$5,$6)";

    std::vector<StatementTestTable> const v(2);
    ASSERT_EQ(query, RangeStatement::insert(v.begin(), v.end()));
    ASSERT_EQ("($2,$3,$4),($5,$6,$7)", RangeStatement::placeholders(v.begin(), v.end(), 1));
}

}  // namespace postgres
