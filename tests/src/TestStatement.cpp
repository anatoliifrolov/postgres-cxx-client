#include <string>
#include <vector>
#include <gtest/gtest.h>
#include <postgres/Statement.h>
#include <postgres/Visitable.h>

namespace postgres {

struct STable {
    int a = 0;
    int b = 0;
    int c = 0;

    POSTGRES_CXX_TABLE("test", a, b, c)
};

TEST(Statement, All) {
    std::vector<STable> const v(3);
    std::string const         single = "INSERT INTO test (a,b,c) VALUES ($1,$2,$3)";
    auto const                multi  = single + ",($4,$5,$6),($7,$8,$9)";
    ASSERT_EQ(single, Statement<STable>::insert());
    ASSERT_EQ(multi, RangeStatement::insert(v.begin(), v.end()));
    ASSERT_EQ("UPDATE test SET a=$1,b=$2,c=$3", Statement<STable>::update());
    ASSERT_EQ("SELECT a,b,c FROM test", Statement<STable>::select());
    ASSERT_EQ("a,b,c", Statement<STable>::fields());
    ASSERT_EQ("$1,$2,$3", Statement<STable>::placeholders());
    ASSERT_EQ("a=$1,b=$2,c=$3", Statement<STable>::assignments());
}

}  // namespace postgres
