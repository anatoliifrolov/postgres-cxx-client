#include <vector>
#include <set>
#include <gtest/gtest.h>
#include <postgres/Command.h>
#include "Migration.h"
#include "TimestampSamples.h"

namespace postgres {

struct TestVisitor : Migration, testing::Test {};

TEST_F(TestVisitor, Manual) {
    test pinged{};
    pinged.int2 = 2;
    pinged.int4 = 4;
    pinged.int8 = 8;
    pinged.float4 = 4.44;
    pinged.float8 = 8.88;
    pinged.flag = true;
    pinged.info = "INFO";
    pinged.time = timePointSample();

    test ponged{};
    const auto res = client_.execute(
        Command{
            R"(INSERT INTO test(int2, int4, int8, float4, float8, flag, info, time)
                VALUES($1, $2, $3, $4, $5, $6, $7, $8))",
            pinged},
        "SELECT int2, int4, int8, float4, float8, flag, info, time FROM test");
    res.front() >> ponged;

    ASSERT_EQ(2, ponged.int2);
    ASSERT_EQ(4, ponged.int4);
    ASSERT_EQ(8, ponged.int8);
    ASSERT_NEAR(4.44, ponged.float4, 0.001);
    ASSERT_NEAR(8.88, ponged.float8, 0.001);
    ASSERT_TRUE(ponged.flag);
    ASSERT_EQ("INFO", ponged.info);
    ASSERT_EQ(timePointSample(), ponged.time);
}

static std::vector<test> makeDataToInsert() {
    std::vector<test> data{};

    data.emplace_back();
    data.back().int2 = 2;
    data.back().int4 = 4;
    data.back().int8 = 8;
    data.back().float4 = 4.44;
    data.back().float8 = 8.88;
    data.back().flag = true;
    data.back().info = "INFO";
    data.back().time = timePointSample();

    data.emplace_back();
    data.back().int2 = 22;
    data.back().int4 = 24;
    data.back().int8 = 28;
    data.back().float4 = 24.44;
    data.back().float8 = 28.88;
    data.back().flag = true;
    data.back().info = "INFO2";
    data.back().time = timePointSample();

    data.emplace_back();
    data.back().int2 = 32;
    data.back().int4 = 34;
    data.back().int8 = 38;
    data.back().float4 = 34.44;
    data.back().float8 = 38.88;
    data.back().flag = true;
    data.back().info = "INFO3";
    data.back().time = timePointSample();

    data.emplace_back();
    data.back().int2 = 42;
    data.back().int4 = 44;
    data.back().int8 = 48;
    data.back().float4 = 44.44;
    data.back().float8 = 48.88;
    data.back().flag = true;
    data.back().info = "INFO4";
    data.back().time = timePointSample();

    return data;
}

TEST_F(TestVisitor, AutoInsert) {
    auto data = makeDataToInsert();
    client_.insert(data[0]);
    client_.insert(data.begin() + 1, data.begin() + 2);
    client_.insert(data.begin() + 2, data.end());

    data.clear();
    const auto res = client_.select(data);
    ASSERT_EQ(4, res.size());
    ASSERT_EQ(4u, data.size());
    ASSERT_EQ(
        (std::set<int16_t>{2, 22, 32, 42}),
        (std::set<int16_t>{data[0].int2, data[1].int2, data[2].int2, data[3].int2}));
    ASSERT_EQ(
        (std::set<int32_t>{4, 24, 34, 44}),
        (std::set<int32_t>{data[0].int4, data[1].int4, data[2].int4, data[3].int4}));
    ASSERT_EQ(
        (std::set<int64_t>{8, 28, 38, 48}),
        (std::set<int64_t>{data[0].int8, data[1].int8, data[2].int8, data[3].int8}));
    // Seems to work...
}

TEST_F(TestVisitor, AutoInsertWeak) {
    client_.execute("ALTER TABLE test ADD PRIMARY KEY(int2)");
    auto data = makeDataToInsert();
    const auto it = data.begin() + 1;
    const auto it2 = data.begin() + 2;
    ASSERT_EQ(1, client_.insertWeak(data[0]).affected());
    ASSERT_EQ(0, client_.insertWeak(data[0]).affected());
    ASSERT_THROW(client_.insert(data[0]), std::exception);
    ASSERT_EQ(1, client_.insertWeak(it, it2).affected());
    ASSERT_EQ(0, client_.insertWeak(it, it2).affected());
    ASSERT_THROW(client_.insert(it, it2), std::exception);
    ASSERT_EQ(2, client_.insertWeak(it2, data.end()).affected());
    ASSERT_EQ(0, client_.insertWeak(it2, data.end()).affected());
    ASSERT_THROW(client_.insert(it2, data.end()), std::exception);

    data.clear();
    const auto res = client_.select(data);
    ASSERT_EQ(4, res.size());
    ASSERT_EQ(4u, data.size());
    ASSERT_EQ(
        (std::set<int16_t>{2, 22, 32, 42}),
        (std::set<int16_t>{data[0].int2, data[1].int2, data[2].int2, data[3].int2}));
    ASSERT_EQ(
        (std::set<int32_t>{4, 24, 34, 44}),
        (std::set<int32_t>{data[0].int4, data[1].int4, data[2].int4, data[3].int4}));
    ASSERT_EQ(
        (std::set<int64_t>{8, 28, 38, 48}),
        (std::set<int64_t>{data[0].int8, data[1].int8, data[2].int8, data[3].int8}));
    // Seems to work...
}

TEST_F(TestVisitor, AutoUpdate) {
    std::vector<test> data{};

    data.emplace_back();
    data.back().int2 = 2;
    data.back().int4 = 4;
    data.back().int8 = 8;
    data.back().float4 = 4.44;
    data.back().float8 = 8.88;
    data.back().flag = true;
    data.back().info = "INFO";
    data.back().time = timePointSample();
    client_.insert(data.back());

    data.back().int2 = 22;
    data.back().int4 = 24;
    data.back().int8 = 28;
    data.back().float4 = 24.44;
    data.back().float8 = 28.88;
    data.back().flag = false;
    data.back().info = "INFO2";
    data.back().time = timePointSample();
    client_.update(data.back());

    data.clear();
    const auto res = client_.select(data);

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
