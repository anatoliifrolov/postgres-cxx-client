#include <vector>
#include <set>
#include <gtest/gtest.h>
#include <postgres/Command.h>
#include <postgres/Row.h>
#include "Migration.h"
#include "Timestamps.h"

namespace postgres {

struct TestVisitor : Migration, testing::Test {
};

TEST_F(TestVisitor, Manual) {
    Table pinged{};
    pinged.int2   = 2;
    pinged.int4   = 4;
    pinged.int8   = 8;
    pinged.float4 = 4.44;
    pinged.float8 = 8.88;
    pinged.flag   = true;
    pinged.info   = "INFO";
    pinged.time   = timePointSample();

    Table ponged{};

    auto res = conn_->exec(Command{R"(INSERT INTO test(int2, int4, int8, float4, float8, flag, info, time)
                VALUES($1, $2, $3, $4, $5, $6, $7, $8))", pinged}).valid();
    res = conn_->exec(Command{"SELECT int2, int4, int8, float4, float8, flag, info, time FROM test"});
    res[0] >> ponged;

    ASSERT_EQ(2, ponged.int2);
    ASSERT_EQ(4, ponged.int4);
    ASSERT_EQ(8, ponged.int8);
    ASSERT_NEAR(4.44, ponged.float4, 0.001);
    ASSERT_NEAR(8.88, ponged.float8, 0.001);
    ASSERT_TRUE(ponged.flag);
    ASSERT_EQ("INFO", ponged.info);
    ASSERT_EQ(timePointSample(), ponged.time);
}

static std::vector<Table> makeDataToInsert() {
    std::vector<Table> data{};

    data.emplace_back();
    data.back().int2   = 2;
    data.back().int4   = 4;
    data.back().int8   = 8;
    data.back().float4 = 4.44;
    data.back().float8 = 8.88;
    data.back().flag   = true;
    data.back().info   = "INFO";
    data.back().time   = timePointSample();

    data.emplace_back();
    data.back().int2   = 22;
    data.back().int4   = 24;
    data.back().int8   = 28;
    data.back().float4 = 24.44;
    data.back().float8 = 28.88;
    data.back().flag   = true;
    data.back().info   = "INFO2";
    data.back().time   = timePointSample();

    data.emplace_back();
    data.back().int2   = 32;
    data.back().int4   = 34;
    data.back().int8   = 38;
    data.back().float4 = 34.44;
    data.back().float8 = 38.88;
    data.back().flag   = true;
    data.back().info   = "INFO3";
    data.back().time   = timePointSample();

    data.emplace_back();
    data.back().int2   = 42;
    data.back().int4   = 44;
    data.back().int8   = 48;
    data.back().float4 = 44.44;
    data.back().float8 = 48.88;
    data.back().flag   = true;
    data.back().info   = "INFO4";
    data.back().time   = timePointSample();

    return data;
}

TEST_F(TestVisitor, AutoInsert) {
    auto data = makeDataToInsert();
    conn_->insert(data[0]);
    conn_->insert(data.begin() + 1, data.begin() + 2);
    conn_->insert(data.begin() + 2, data.end());

    data.clear();
    auto const res = conn_->select(data);
    ASSERT_EQ(4, res.size());
    ASSERT_EQ(4u, data.size());
    ASSERT_EQ((std::set<int16_t>{2, 22, 32, 42}),
              (std::set<int16_t>{data[0].int2, data[1].int2, data[2].int2, data[3].int2}));
    ASSERT_EQ((std::set<int32_t>{4, 24, 34, 44}),
              (std::set<int32_t>{data[0].int4, data[1].int4, data[2].int4, data[3].int4}));
    ASSERT_EQ((std::set<int64_t>{8, 28, 38, 48}),
              (std::set<int64_t>{data[0].int8, data[1].int8, data[2].int8, data[3].int8}));
    // Seems to work...
}

TEST_F(TestVisitor, AutoUpdate) {
    std::vector<Table> data{};

    data.emplace_back();
    data.back().int2   = 2;
    data.back().int4   = 4;
    data.back().int8   = 8;
    data.back().float4 = 4.44;
    data.back().float8 = 8.88;
    data.back().flag   = true;
    data.back().info   = "INFO";
    data.back().time   = timePointSample();
    conn_->insert(data.back());

    data.back().int2   = 22;
    data.back().int4   = 24;
    data.back().int8   = 28;
    data.back().float4 = 24.44;
    data.back().float8 = 28.88;
    data.back().flag   = false;
    data.back().info   = "INFO2";
    data.back().time   = timePointSample();
    conn_->update(data.back());

    data.clear();
    auto const res = conn_->select(data);

    ASSERT_EQ(1u, data.size());
    ASSERT_EQ(22, data.back().int2);
    ASSERT_EQ(24, data.back().int4);
    ASSERT_EQ(28, data.back().int8);
    ASSERT_NEAR(24.44, data.back().float4, 0.001);
    ASSERT_NEAR(28.88, data.back().float8, 0.001);
    ASSERT_FALSE(data.back().flag);
    ASSERT_EQ("INFO2", data.back().info);
    ASSERT_EQ(timePointSample(), data.back().time);
}

}  // namespace postgres
