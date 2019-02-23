#include <gtest/gtest.h>
#include <postgres/Command.h>
#include <postgres/Result.h>
#include <postgres/Row.h>
#include "Migration.h"

namespace postgres {

struct TestResult : Migration, testing::Test {
};

TEST_F(TestResult, NullPtr) {
    Result res{nullptr};
    res.type();
    res.describe();
    res.message();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isDone());
    ASSERT_EQ(0, res.size());
    ASSERT_EQ(0, res.effect());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_THROW(res.begin(), std::exception);
    ASSERT_THROW(res.end(), std::exception);
    ASSERT_THROW(res[0], std::exception);
}

TEST_F(TestResult, Invalid) {
    auto const res = conn_->exec(Command{"BAD STATEMENT"});
    ASSERT_FALSE(res.isOk());
    ASSERT_EQ(0, res.size());
    ASSERT_EQ(0, res.effect());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_THROW(res.begin(), std::exception);
    ASSERT_THROW(res.end(), std::exception);
    ASSERT_THROW(res[0], std::exception);
}

TEST_F(TestResult, Empty) {
    auto const res = conn_->exec(Command{"SELECT * FROM test"});
    ASSERT_TRUE(res.isOk());
    ASSERT_EQ(0, res.size());
    ASSERT_EQ(0, res.effect());
    ASSERT_TRUE(res.isEmpty());

    for (auto const tuple : res) {
        // Must not reach here.
        ASSERT_FALSE((bool) "Iterating empty result");
    }
}

TEST_F(TestResult, Survival) {
    auto const res = conn_->exec(Command{"SELECT 123::INTEGER"});
    {
        conn_->~Connection();
    }
    ASSERT_TRUE(res.isOk());
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(123, res[0][0].as<int>());
}

TEST_F(TestResult, Iteration) {
    std::vector<int32_t> data{};
    conn_->exec(Command{"INSERT INTO test(int4) VALUES(1), (2), (3)"});
    for (auto tuple : conn_->exec(Command{"SELECT int4 FROM test"})) {
        data.emplace_back();
        tuple >> data.back();
    }
    ASSERT_EQ(3u, data.size());
    ASSERT_EQ(1, data[0]);
    ASSERT_EQ(2, data[1]);
    ASSERT_EQ(3, data[2]);
}

TEST_F(TestResult, ColumnName) {
    auto const res = conn_->exec(Command{"SELECT 1 AS one, 2 AS two"});
    ASSERT_STREQ("one", res[0][0].name());
    ASSERT_STREQ("two", res[0][1].name());
}

}  // namespace postgres
