#include <stdexcept>
#include <utility>
#include <gtest/gtest.h>
#include <postgres/Transaction.h>
#include <postgres/Command.h>
#include <postgres/Row.h>
#include "Migration.h"

namespace postgres {

struct TestTransaction : Migration, testing::Test {
};

TEST_F(TestTransaction, InvalidUse) {
    auto transaction  = conn_->begin();
    auto transaction2 = std::move(transaction);
    ASSERT_THROW(transaction.commit(), std::exception);
    transaction2.commit();
    ASSERT_THROW(transaction2.commit(), std::exception);
}

TEST_F(TestTransaction, Commit) {
    auto transaction = conn_->begin();
    conn_->exec("INSERT INTO test(flag) VALUES(TRUE)");
    transaction.commit();
    auto const res = conn_->exec("SELECT flag FROM test");
    ASSERT_EQ(true, res[0][0].as<bool>());
}

TEST_F(TestTransaction, Rollback) {
    {
        auto transaction = conn_->begin();
        conn_->exec("INSERT INTO test(flag) VALUES(TRUE)");
    }
    auto const res = conn_->exec("SELECT flag FROM test");
    ASSERT_TRUE(res.isOk());
    ASSERT_EQ(0, res.size());
}

TEST_F(TestTransaction, AutoCommit) {
    auto res = client_.transact("INSERT INTO test(int4) VALUES(1)",
                                "INSERT INTO test(int4) VALUES(2), (3)");
    ASSERT_EQ(2, res.effect());
    res = conn_->exec("SELECT int4 FROM test ORDER BY int4");
    ASSERT_EQ(1, res[0][0].as<int>());
    ASSERT_EQ(2, res[1][0].as<int>());
    ASSERT_EQ(3, res[2][0].as<int>());
}

TEST_F(TestTransaction, AutoRollback) {
    ASSERT_THROW(client_.transact("INSERT INTO test(int4) VALUES(1)", "BAD STATEMENT").valid(),
                 std::exception);
    auto res = client_.transact("INSERT INTO test(int4) VALUES(1)",
                                "BAD STATEMENT",
                                "INSERT INTO test(int4) VALUES(2), (3)");
    ASSERT_FALSE(res.isOk());
    res = conn_->exec("SELECT int4 FROM test");
    ASSERT_TRUE(res.isOk());
    ASSERT_EQ(0, res.size());
}

}  // namespace postgres
