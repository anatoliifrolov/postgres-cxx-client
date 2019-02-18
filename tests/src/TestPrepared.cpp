#include <gtest/gtest.h>
#include <postgres/PreparedCommand.h>
#include <postgres/PrepareData.h>
#include <postgres/Tuple.h>
#include "Migration.h"

namespace postgres {

struct TestPrepared : Migration, testing::Test {
    TestPrepared() {
        client_.prepare(PrepareData{"prepared_insert", "INSERT INTO test(flag) VALUES($1)"});
    }
};

TEST_F(TestPrepared, Basic) {
    auto res = client_.execute(PreparedCommand{"prepared_insert", true}, "SELECT flag FROM test");
    ASSERT_EQ(true, (bool) res[0][0]);
}

}  // namespace postgres
