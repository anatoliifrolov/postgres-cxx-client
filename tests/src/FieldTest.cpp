#include <cstdint>
#include <optional>
#include <gtest/gtest.h>
#include <postgres/Connection.h>
#include <postgres/Error.h>
#include <postgres/Field.h>
#include "Timestamps.h"

namespace postgres {

TEST(FieldTest, Meta) {
    auto const res = Connection{}.exec("SELECT 'foo'::TEXT AS bar");
    auto const fld = res[0][0];
    ASSERT_FALSE(fld.isNull());
    ASSERT_STREQ("bar", fld.name());
    ASSERT_STREQ("foo", fld.value());
    ASSERT_EQ(TEXTOID, fld.type());
    ASSERT_EQ(3, fld.length());
    ASSERT_EQ(1, fld.format());
}

TEST(FieldTest, MetaNull) {
    auto const res = Connection{}.exec("SELECT NULL");
    auto const fld = res[0][0];
    ASSERT_TRUE(fld.isNull());
}

TEST(FieldTest, Arithm) {
    ASSERT_EQ(true, Connection{}.exec("SELECT TRUE")[0][0].as<bool>());
    ASSERT_EQ(false, Connection{}.exec("SELECT FALSE")[0][0].as<bool>());
    ASSERT_EQ(1, Connection{}.exec("SELECT 1::SMALLINT")[0][0].as<int16_t>());
    ASSERT_EQ(2, Connection{}.exec("SELECT 2::INT")[0][0].as<int32_t>());
    ASSERT_EQ(3, Connection{}.exec("SELECT 3::BIGINT")[0][0].as<int64_t>());
    ASSERT_NEAR(4.56, Connection{}.exec("SELECT 4.56::REAL")[0][0].as<float>(), 0.001);
    ASSERT_NEAR(5.67, Connection{}.exec("SELECT 5.67::DOUBLE PRECISION")[0][0].as<double>(), 0.001);
}

TEST(FieldTest, ArithmLarge) {
    ASSERT_EQ(1, Connection{}.exec("SELECT 1::SMALLINT")[0][0].as<int32_t>());
    ASSERT_EQ(2, Connection{}.exec("SELECT 2::INT")[0][0].as<int64_t>());
    ASSERT_NEAR(4.56, Connection{}.exec("SELECT 4.56::REAL")[0][0].as<double>(), 0.001);
}

TEST(FieldTest, ArithmNarrow) {
    ASSERT_THROW(Connection{}.exec("SELECT 1::SMALLINT")[0][0].as<int8_t>(), Error);
    ASSERT_THROW(Connection{}.exec("SELECT 2::INT")[0][0].as<int16_t>(), Error);
    ASSERT_THROW(Connection{}.exec("SELECT 3::BIGINT")[0][0].as<int32_t>(), Error);
    ASSERT_THROW(Connection{}.exec("SELECT 4.56::DOUBLE PRECISION")[0][0].as<float>(), Error);
}

TEST(FieldTest, ArithmUnsigned) {
    ASSERT_EQ(1, Connection{}.exec("SELECT 1::SMALLINT")[0][0].as<uint16_t>());
    ASSERT_EQ(2, Connection{}.exec("SELECT 2::INT")[0][0].as<uint32_t>());
    ASSERT_EQ(3, Connection{}.exec("SELECT 3::BIGINT")[0][0].as<uint64_t>());
    ASSERT_THROW(Connection{}.exec("SELECT -1::SMALLINT")[0][0].as<uint16_t>(), Error);
    ASSERT_THROW(Connection{}.exec("SELECT -2::INT")[0][0].as<uint32_t>(), Error);
    ASSERT_THROW(Connection{}.exec("SELECT -3::BIGINT")[0][0].as<uint64_t>(), Error);
}

TEST(FieldTest, ArithmRound) {
    ASSERT_THROW(Connection{}.exec("SELECT 3::BIGINT")[0][0].as<double>(), Error);
    ASSERT_THROW(Connection{}.exec("SELECT 4.56::DOUBLE PRECISION")[0][0].as<int64_t>(), Error);
}

TEST(FieldTest, ArithmUnsupport) {
    ASSERT_THROW(Connection{}.exec("SELECT 3::DECIMAL")[0][0].as<long double>(), Error);
    ASSERT_THROW(Connection{}.exec("SELECT 4::NUMERIC")[0][0].as<long double>(), Error);
}

TEST(FieldTest, Str) {
    ASSERT_EQ("foo", Connection{}.exec("SELECT 'foo'")[0][0].as<std::string>());
}

TEST(FieldTest, Time) {
    auto const res = Connection{}.exec("SELECT '2017-08-25 13:03:35'::TIMESTAMP");
    auto const fld = res[0][0];
    ASSERT_EQ(timeSample(), fld.as<Time>().toUnix());
    ASSERT_EQ(timePointSample(), fld.as<Time::Point>());
}

TEST(FieldTest, TimeBad) {
    ASSERT_THROW(Connection{}.exec("SELECT '2017-08-25 13:03:35'")[0][0].as<Time>().toUnix(), Error);
}

TEST(FieldTest, Assign) {
    int32_t val = 0;
    Connection{}.exec("SELECT 1::INT")[0][0] >> val;
    ASSERT_EQ(1, val);
}

TEST(FieldTest, AssignNull) {
    int32_t val = 0;
    ASSERT_THROW(Connection{}.exec("SELECT NULL")[0][0] >> val, Error);
}

TEST(FieldTest, Ptr) {
    int32_t val = 0;
    auto    ptr = &val;
    Connection{}.exec("SELECT 1::INT")[0][0] >> ptr;
    ASSERT_NE(nullptr, ptr);
    ASSERT_EQ(1, *ptr);
}

TEST(FieldTest, PtrNull) {
    int32_t val = 0;
    auto    ptr = &val;
    Connection{}.exec("SELECT NULL")[0][0] >> ptr;
    ASSERT_EQ(nullptr, ptr);
}

TEST(FieldTest, Opt) {
    std::optional<int32_t> opt{};
    Connection{}.exec("SELECT 1::INT")[0][0] >> opt;
    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(1, opt.value());
}

TEST(FieldTest, OptNull) {
    std::optional<int32_t> opt;
    Connection{}.exec("SELECT NULL")[0][0] >> opt;
    ASSERT_FALSE(opt.has_value());
}

TEST(FieldTest, OptAs) {
    auto const opt = Connection{}.exec("SELECT 1::INT")[0][0].as<std::optional<int32_t>>();
    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(1, opt.value());
}

TEST(FieldTest, OptAsNull) {
    auto const opt = Connection{}.exec("SELECT NULL")[0][0].as<std::optional<int32_t>>();
    ASSERT_FALSE(opt.has_value());
}

}  // namespace postgres
