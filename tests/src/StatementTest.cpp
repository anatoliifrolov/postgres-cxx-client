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
    std::string const single = "INSERT INTO test (a,b,c) VALUES ($1,$2,$3)";
    auto const        multi  = single + ",($4,$5,$6),($7,$8,$9)";

    std::vector<StatementTestTable> const v(3);
    ASSERT_EQ(single, Statement<StatementTestTable>::insert());
    ASSERT_EQ(multi, RangeStatement::insert(v.begin(), v.end()));
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
    ASSERT_EQ("a=$1,b=$2,c=$3", Statement<StatementTestTable>::assignments());
}

}  // namespace postgres
