#include <gtest/gtest.h>
#include <postgres/Command.h>
#include "Migration.h"
#include "TimestampSamples.h"

namespace postgres {

struct TestData : Migration, testing::Test {
};

TEST_F(TestData, Null) {
    const bool* const some_ptr = nullptr;
    auto const res = client_.execute(Command{"INSERT INTO test(flag) VALUES(NULL), ($1), ($2)",
                                             nullptr,
                                             some_ptr}, "SELECT flag FROM test");

    // Reading NULLs into pointers allowed only.
    auto val = true;
    ASSERT_THROW(res[0] >> val, std::exception);
    ASSERT_THROW(res[1] >> val, std::exception);
    ASSERT_THROW(res[2] >> val, std::exception);

    for (auto const i : {0, 1, 2}) {
        auto ptr = &val;
        res[i] >> ptr;
        ASSERT_EQ(nullptr, ptr);
    }
}

TEST_F(TestData, NullOptional) {
    std::optional<int> opt;
    client_.execute(Command{"INSERT INTO test (int4) VALUES ($1)", opt},
                    "SELECT int4 FROM test")[0][0] >> opt;
    ASSERT_FALSE(opt.has_value());
}

TEST_F(TestData, OptionalValue) {
    std::optional<int> opt, opt2;
    opt = 2;
    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(2, opt.value());
    ASSERT_FALSE(opt2.has_value());
    client_.execute(Command{"INSERT INTO test (int4) VALUES ($1)", opt},
                    "SELECT int4 FROM test")[0][0] >> opt2;
    ASSERT_TRUE(opt2.has_value());
    ASSERT_EQ(2, opt2.value());
}

TEST_F(TestData, OptionalTuple) {
    std::optional<int> opt, opt2;
    client_.execute("SELECT 2, NULL::INT")[0] >> opt >> opt2;
    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(2, opt.value());
    ASSERT_FALSE(opt2.has_value());
}

TEST_F(TestData, Types) {
    auto const res = client_.execute(Command{
        R"(INSERT INTO test(int2, int4, int8, float4, float8, flag, info) VALUES($1, $2, $3, $4, $5, $6, $7))",
        int16_t{2},
        int32_t{4},
        int64_t{8},
        float{4.44},
        8.88,
        true,
        "INFO"}, "SELECT int2, int4, int8, float4, float8, flag, info FROM test");

    test row{};

    // Normal.
    res[0]["int2"] >> row.int2;
    ASSERT_EQ(2, row.int2);
    res[0]["int4"] >> row.int4;
    ASSERT_EQ(4, row.int4);
    res[0]["int8"] >> row.int8;
    ASSERT_EQ(8, row.int8);
    res[0]["float4"] >> row.float4;
    ASSERT_NEAR(4.44, row.float4, 0.001);
    res[0]["float8"] >> row.float8;
    ASSERT_NEAR(8.88, row.float8, 0.001);
    res[0]["flag"] >> row.flag;
    ASSERT_EQ(true, row.flag);
    res[0]["info"] >> row.info;
    ASSERT_EQ("INFO", row.info);
    ASSERT_THROW(res[0]["time"] >> row.time, std::exception);

    // Smaller type into larger.
    res[0]["int2"] >> row.int4;
    ASSERT_EQ(2, row.int4);
    res[0]["int4"] >> row.int8;
    ASSERT_EQ(4, row.int8);
    res[0]["float4"] >> row.float8;
    ASSERT_NEAR(4.44, row.float8, 0.001);

    // Cast.
    res[0]["int8"] >> row.float8;
    ASSERT_DOUBLE_EQ(8, row.float8);
    res[0]["float8"] >> row.int8;
    ASSERT_EQ(8, row.int8);

    // Narrowing.
    ASSERT_THROW(res[0]["int2"] >> row.flag, std::exception);
    ASSERT_THROW(res[0]["int4"] >> row.int2, std::exception);
    ASSERT_THROW(res[0]["int8"] >> row.int4, std::exception);
    ASSERT_THROW(res[0]["float8"] >> row.float4, std::exception);
}

TEST_F(TestData, Timestamp) {
    time_t                                time{};
    std::chrono::system_clock::time_point time_point{};

    auto const res = client_.execute(Command{"INSERT INTO test(time) VALUES($1)",
                                             timeSampleFormatPrecise()}, "SELECT time FROM test");

    res[0][0] >> time;
    ASSERT_EQ(timeSample(), time);
    res[0][0] >> time_point;
    ASSERT_EQ(timePointSampleUs(), time_point);
}

TEST_F(TestData, Esc) {
    auto const res = client_.execute(Command{"INSERT INTO test(info) VALUES($1)",
                                             "'QUOTED_STRING's"}, "SELECT info FROM test");

    const std::string s = res[0][0];
    ASSERT_EQ("'QUOTED_STRING's", s);
}

TEST_F(TestData, MultiRef) {
    auto const res = client_.execute(Command{"INSERT INTO test(int2, int4, int8) VALUES($1, $1, $1)",
                                             2}, "SELECT int2, int4, int8 FROM test");
    ASSERT_EQ(2, (int16_t) res[0][0]);
    ASSERT_EQ(2, (int32_t) res[0][1]);
    ASSERT_EQ(2, (int64_t) res[0][2]);
}

}  // namespace postgres
