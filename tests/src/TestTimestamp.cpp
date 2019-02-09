#include <stdexcept>
#include <gtest/gtest.h>
#include <postgres/Timestamp.h>
#include "TimestampSamples.h"

namespace postgres {

TEST(TestTimestamp, CStyle) {
    const auto ts = makeTimestamp(timeSample());
    ASSERT_EQ(timeSample(), ts.unixTime());
    ASSERT_EQ(timeSamplePg(), ts.postgresTime());
    ASSERT_EQ(timePointSample(), ts.timePoint());
    ASSERT_EQ(timeSampleFormat(), ts.format());
    ASSERT_FALSE(ts.hasTimezone());
}

TEST(TestTimestamp, Chrono) {
    const auto ts = makeTimestamp(timePointSampleNs());
    ASSERT_EQ(timeSample(), ts.unixTime());
    ASSERT_EQ(timeSamplePgUs(), ts.postgresTime());
    ASSERT_EQ(timePointSampleNs(), ts.timePoint());
    ASSERT_EQ(timeSampleFormatPrecise(), ts.format());
    ASSERT_FALSE(ts.hasTimezone());
}

TEST(TestTimestamp, Tz) {
    const auto ts = makeTimestamp(timePointSampleNs(), true);
    ASSERT_EQ(timeSample(), ts.unixTime());
    ASSERT_EQ(timeSamplePgUs(), ts.postgresTime());
    ASSERT_EQ(timePointSampleNs(), ts.timePoint());
    ASSERT_EQ(timeSampleFormatPreciseTz(), ts.format());
    ASSERT_TRUE(ts.hasTimezone());
}

TEST(TestTimestamp, Format) {
    const auto ts = makeTimestamp(timeSampleFormat());
    ASSERT_EQ(timeSample(), ts.unixTime());
    ASSERT_EQ(timeSamplePg(), ts.postgresTime());
    ASSERT_EQ(timePointSample(), ts.timePoint());
    ASSERT_EQ(timeSampleFormat(), ts.format());
    ASSERT_FALSE(ts.hasTimezone());
}

TEST(TestTimestamp, FormatPrecise) {
    const auto ts = makeTimestamp(timeSampleFormatPrecise());
    ASSERT_EQ(timeSample(), ts.unixTime());
    ASSERT_EQ(timeSamplePgUs(), ts.postgresTime());
    ASSERT_EQ(timePointSampleNs(), ts.timePoint());
    ASSERT_EQ(timeSampleFormatPrecise(), ts.format());
    ASSERT_FALSE(ts.hasTimezone());
}

TEST(TestTimestamp, FormatIn) {
    const auto ts = makeTimestamp("2017-08-25 13:03:35");
    ASSERT_EQ(timeSample(), ts.unixTime());
    ASSERT_EQ(timeSamplePg(), ts.postgresTime());
    ASSERT_EQ(timePointSample(), ts.timePoint());
    ASSERT_EQ(timeSampleFormat(), ts.format());
    ASSERT_FALSE(ts.hasTimezone());
}

TEST(TestTimestamp, FormatPreciseIn) {
    const auto ts = makeTimestamp("2017-08-25 13:03:35.987654");
    ASSERT_EQ(timeSample(), ts.unixTime());
    ASSERT_EQ(timeSamplePgUs(), ts.postgresTime());
    ASSERT_EQ(timePointSampleUs(), ts.timePoint());
    ASSERT_EQ("2017-08-25T13:03:35.987654000", ts.format());
    ASSERT_FALSE(ts.hasTimezone());
}

TEST(TestTimestamp, FormatInvalid) {
    ASSERT_THROW(makeTimestamp("BAD TIMESTAMP"), std::exception);
    ASSERT_THROW(makeTimestamp("2017-13-25 13:03:35"), std::exception);
    ASSERT_THROW(makeTimestamp("2017-08-32 13:03:35"), std::exception);
    ASSERT_THROW(makeTimestamp("2017-08-25 25:03:35"), std::exception);
    ASSERT_THROW(makeTimestamp("2017-08-25 13:60:35"), std::exception);
    ASSERT_THROW(makeTimestamp("2017-08-25 13:03:60"), std::exception);
}

TEST(TestTimestamp, FormatNegative) {
    static constexpr auto time = -4807767385l;
    static const auto format = "1817-08-25T13:03:35";
    const auto ts = makeTimestamp(format);
    ASSERT_EQ(time, ts.unixTime());
    ASSERT_EQ(std::chrono::system_clock::from_time_t(time), ts.timePoint());
    ASSERT_EQ(format, ts.format());
    ASSERT_FALSE(ts.hasTimezone());
}

}  // namespace postgres
