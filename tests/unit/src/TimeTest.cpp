#include <stdexcept>
#include <gtest/gtest.h>
#include <postgres/Error.h>
#include <postgres/Time.h>
#include "Samples.h"

namespace postgres {

TEST(TimeTest, CStyle) {
    auto const t = Time{TIME_SAMPLE};
    ASSERT_EQ(TIME_SAMPLE, t.toUnix());
    ASSERT_EQ(TIME_SAMPLE_PG, t.toPostgres());
    ASSERT_EQ(TIME_POINT_SAMPLE, t.point());
    ASSERT_EQ(TIME_STR_SAMPLE, t.toString());
    ASSERT_FALSE(t.hasZone());
}

TEST(TimeTest, Chrono) {
    auto const t = Time{TIME_POINT_SAMPLE_NANO};
    ASSERT_EQ(TIME_SAMPLE, t.toUnix());
    ASSERT_EQ(TIME_SAMPLE_PG_MICRO, t.toPostgres());
    ASSERT_EQ(TIME_POINT_SAMPLE_NANO, t.point());
    ASSERT_EQ(TIME_STR_SAMPLE_NANO, t.toString());
    ASSERT_FALSE(t.hasZone());
}

TEST(TimeTest, Tz) {
    auto const t = Time{TIME_POINT_SAMPLE_NANO, true};
    ASSERT_EQ(TIME_SAMPLE, t.toUnix());
    ASSERT_EQ(TIME_SAMPLE_PG_MICRO, t.toPostgres());
    ASSERT_EQ(TIME_POINT_SAMPLE_NANO, t.point());
    ASSERT_EQ(TIME_STR_SAMPLE_NANO_TZ, t.toString());
    ASSERT_TRUE(t.hasZone());
}

TEST(TimeTest, Fmt) {
    auto const t = Time{TIME_STR_SAMPLE};
    ASSERT_EQ(TIME_SAMPLE, t.toUnix());
    ASSERT_EQ(TIME_SAMPLE_PG, t.toPostgres());
    ASSERT_EQ(TIME_POINT_SAMPLE, t.point());
    ASSERT_EQ(TIME_STR_SAMPLE, t.toString());
    ASSERT_FALSE(t.hasZone());
}

TEST(TimeTest, NanoFmt) {
    auto const t = Time{TIME_STR_SAMPLE_NANO};
    ASSERT_EQ(TIME_SAMPLE, t.toUnix());
    ASSERT_EQ(TIME_SAMPLE_PG_MICRO, t.toPostgres());
    ASSERT_EQ(TIME_POINT_SAMPLE_NANO, t.point());
    ASSERT_EQ(TIME_STR_SAMPLE_NANO, t.toString());
    ASSERT_FALSE(t.hasZone());
}

TEST(TimeTest, SpaceDelim) {
    auto const t = Time{"2017-08-25 13:03:35"};
    ASSERT_EQ(TIME_SAMPLE, t.toUnix());
    ASSERT_EQ(TIME_SAMPLE_PG, t.toPostgres());
    ASSERT_EQ(TIME_POINT_SAMPLE, t.point());
    ASSERT_EQ(TIME_STR_SAMPLE, t.toString());
    ASSERT_FALSE(t.hasZone());
}

TEST(TimeTest, SpaceDelimMicro) {
    auto const t = Time{"2017-08-25 13:03:35.987654"};
    ASSERT_EQ(TIME_SAMPLE, t.toUnix());
    ASSERT_EQ(TIME_SAMPLE_PG_MICRO, t.toPostgres());
    ASSERT_EQ(TIME_POINT_SAMPLE_MICRO, t.point());
    ASSERT_EQ("2017-08-25T13:03:35.987654000", t.toString());
    ASSERT_FALSE(t.hasZone());
}

TEST(TimeTest, BadFmt) {
    ASSERT_THROW(Time{"BAD TIMESTAMP"}, LogicError);
    ASSERT_THROW(Time{"2017-13-25 13:03:35"}, LogicError);
    ASSERT_THROW(Time{"2017-08-32 13:03:35"}, LogicError);
    ASSERT_THROW(Time{"2017-08-25 25:03:35"}, LogicError);
    ASSERT_THROW(Time{"2017-08-25 13:60:35"}, LogicError);
    ASSERT_THROW(Time{"2017-08-25 13:03:60"}, LogicError);
}

TEST(TimeTest, Past) {
    static auto const fmt = "1817-08-25T13:03:35";
    auto const        t   = Time{fmt};
    auto constexpr    uni = -4807767385l;
    ASSERT_EQ(uni, t.toUnix());
    ASSERT_EQ(std::chrono::system_clock::from_time_t(uni), t.point());
    ASSERT_EQ(fmt, t.toString());
    ASSERT_FALSE(t.hasZone());
}

}  // namespace postgres
