#include <stdexcept>
#include <utility>
#include <gtest/gtest.h>
#include <postgres/Transaction.h>
#include <postgres/Command.h>
#include "Migration.h"

namespace postgres {

struct TestTransaction : Migration, testing::Test {
};

TEST_F(TestTransaction, InvalidUse) {
    auto transaction  = client_.begin();
    auto transaction2 = std::move(transaction);
    ASSERT_THROW(transaction.commit(), std::exception);
    transaction2.commit();
    ASSERT_THROW(transaction2.commit(), std::exception);
}

TEST_F(TestTransaction, Commit) {
    auto transaction = client_.begin();
    client_.execute("INSERT INTO test(flag) VALUES(TRUE)");
    transaction.commit();
    auto const res = client_.execute("SELECT flag FROM test");
    ASSERT_EQ(true, (bool) res[0][0]);
}

TEST_F(TestTransaction, Rollback) {
    {
        auto transaction = client_.begin();
        client_.execute("INSERT INTO test(flag) VALUES(TRUE)");
    }
    auto const res = client_.execute("SELECT flag FROM test");
    ASSERT_TRUE(res.isOk());
    ASSERT_EQ(0, res.size());
}

TEST_F(TestTransaction, AutoCommit) {
    auto res = client_.execute("INSERT INTO test(int4) VALUES(1)",
                               "INSERT INTO test(int4) VALUES(2), (3)");
    ASSERT_EQ(2, res.affected());
    res = client_.execute("SELECT int4 FROM test ORDER BY int4");
    ASSERT_EQ(1, (int) res[0][0]);
    ASSERT_EQ(2, (int) res[1][0]);
    ASSERT_EQ(3, (int) res[2][0]);
}

TEST_F(TestTransaction, AutoRollback) {
    ASSERT_THROW(client_.execute("INSERT INTO test(int4) VALUES(1)", "BAD STATEMENT"),
                 std::exception);
    auto res = client_.tryExecute("INSERT INTO test(int4) VALUES(1)",
                                  "BAD STATEMENT",
                                  "INSERT INTO test(int4) VALUES(2), (3)");
    ASSERT_FALSE(res.isOk());
    res = client_.execute("SELECT int4 FROM test");
    ASSERT_TRUE(res.isOk());
    ASSERT_EQ(0, res.size());
}

}  // namespace postgres
