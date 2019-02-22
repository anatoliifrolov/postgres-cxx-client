#include <stdexcept>
#include <gtest/gtest.h>
#include <postgres/Time.h>
#include "Timestamps.h"

namespace postgres {

TEST(TimeTest, CStyle) {
    auto const t = Time{timeSample()};
    ASSERT_EQ(timeSample(), t.toUnix());
    ASSERT_EQ(timeSamplePg(), t.toPostgres());
    ASSERT_EQ(timePointSample(), t.point());
    ASSERT_EQ(timeFormatSample(), t.toString());
    ASSERT_FALSE(t.hasZone());
}

TEST(TimeTest, Chrono) {
    auto const t = Time{timePointSampleNano()};
    ASSERT_EQ(timeSample(), t.toUnix());
    ASSERT_EQ(timeSamplePgMicro(), t.toPostgres());
    ASSERT_EQ(timePointSampleNano(), t.point());
    ASSERT_EQ(timeFormatSampleNano(), t.toString());
    ASSERT_FALSE(t.hasZone());
}

TEST(TimeTest, Tz) {
    auto const t = Time{timePointSampleNano(), true};
    ASSERT_EQ(timeSample(), t.toUnix());
    ASSERT_EQ(timeSamplePgMicro(), t.toPostgres());
    ASSERT_EQ(timePointSampleNano(), t.point());
    ASSERT_EQ(timeFormatSampleNanoTz(), t.toString());
    ASSERT_TRUE(t.hasZone());
}

TEST(TimeTest, Fmt) {
    auto const t = Time{timeFormatSample()};
    ASSERT_EQ(timeSample(), t.toUnix());
    ASSERT_EQ(timeSamplePg(), t.toPostgres());
    ASSERT_EQ(timePointSample(), t.point());
    ASSERT_EQ(timeFormatSample(), t.toString());
    ASSERT_FALSE(t.hasZone());
}

TEST(TimeTest, NanoFmt) {
    auto const t = Time{timeFormatSampleNano()};
    ASSERT_EQ(timeSample(), t.toUnix());
    ASSERT_EQ(timeSamplePgMicro(), t.toPostgres());
    ASSERT_EQ(timePointSampleNano(), t.point());
    ASSERT_EQ(timeFormatSampleNano(), t.toString());
    ASSERT_FALSE(t.hasZone());
}

TEST(TimeTest, SpaceDelim) {
    auto const t = Time{"2017-08-25 13:03:35"};
    ASSERT_EQ(timeSample(), t.toUnix());
    ASSERT_EQ(timeSamplePg(), t.toPostgres());
    ASSERT_EQ(timePointSample(), t.point());
    ASSERT_EQ(timeFormatSample(), t.toString());
    ASSERT_FALSE(t.hasZone());
}

TEST(TimeTest, SpaceDelimMicro) {
    auto const t = Time{"2017-08-25 13:03:35.987654"};
    ASSERT_EQ(timeSample(), t.toUnix());
    ASSERT_EQ(timeSamplePgMicro(), t.toPostgres());
    ASSERT_EQ(timePointSampleMicro(), t.point());
    ASSERT_EQ("2017-08-25T13:03:35.987654000", t.toString());
    ASSERT_FALSE(t.hasZone());
}

TEST(TimeTest, BadFmt) {
    ASSERT_THROW(Time{"BAD TIMESTAMP"}, std::exception);
    ASSERT_THROW(Time{"2017-13-25 13:03:35"}, std::exception);
    ASSERT_THROW(Time{"2017-08-32 13:03:35"}, std::exception);
    ASSERT_THROW(Time{"2017-08-25 25:03:35"}, std::exception);
    ASSERT_THROW(Time{"2017-08-25 13:60:35"}, std::exception);
    ASSERT_THROW(Time{"2017-08-25 13:03:60"}, std::exception);
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
