#include <vector>
#include <gtest/gtest.h>
#include <postgres/Connection.h>
#include <postgres/Result.h>
#include "Samples.h"

namespace postgres {

TEST(ResultTest, Ok) {
    auto const res = Connection{}.exec("SELECT 1");
    ASSERT_TRUE(res.isOk());
    ASSERT_FALSE(res.isDone());
    ASSERT_FALSE(res.isEmpty());
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(1, res.effect());
    ASSERT_STREQ("", res.message());
    ASSERT_STREQ("PGRES_TUPLES_OK", res.describe());
    ASSERT_EQ(PGRES_TUPLES_OK, res.type());
}

TEST(ResultTest, Empty) {
    auto const res = Connection{}.exec("SELECT 1 WHERE FALSE");
    ASSERT_TRUE(res.isOk());
    ASSERT_FALSE(res.isDone());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_EQ(0, res.size());
    ASSERT_EQ(0, res.effect());
    ASSERT_STREQ("", res.message());
    ASSERT_STREQ("PGRES_TUPLES_OK", res.describe());
    ASSERT_EQ(PGRES_TUPLES_OK, res.type());
}

TEST(ResultTest, Valid) {
    Connection conn{};
    ASSERT_NO_THROW(conn.exec("SELECT 1"));
    ASSERT_THROW(conn.exec("BAD"), RuntimeError);
}

TEST(ResultTest, Range) {
    std::vector<int32_t> vals{};

    for (auto const row : Connection{}.exec(SELECT_MULTI_ROW)) {
        vals.push_back(row[0].as<int32_t>());
    }
    ASSERT_EQ(1, vals[0]);
    ASSERT_EQ(2, vals[1]);
    ASSERT_EQ(3, vals[2]);
}

TEST(ResultTest, RangeEmpty) {
    for (auto const row : Connection{}.exec("SELECT 1 WHERE FALSE")) {
        ASSERT_TRUE(false);
    }
}

TEST(ResultTest, Iter) {
    auto const           res = Connection{}.exec(SELECT_MULTI_ROW);
    std::vector<int32_t> vals{};

    for (auto it = res.begin(); it != res.end(); it++) {
        vals.push_back((*it)[0].as<int32_t>());
    }
    ASSERT_EQ(1, vals[0]);
    ASSERT_EQ(2, vals[1]);
    ASSERT_EQ(3, vals[2]);
}

TEST(ResultTest, Index) {
    auto const res = Connection{}.exec(SELECT_MULTI_ROW);
    ASSERT_EQ(1, res[0][0].as<int32_t>());
    ASSERT_EQ(2, res[1][0].as<int32_t>());
    ASSERT_EQ(3, res[2][0].as<int32_t>());
    ASSERT_THROW(res[3][0].as<int32_t>(), LogicError);
    ASSERT_THROW(res[-1][0].as<int32_t>(), LogicError);
}

}  // namespace postgres
