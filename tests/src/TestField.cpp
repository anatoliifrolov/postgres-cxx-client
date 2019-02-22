#include <cstdint>
#include <optional>
#include <gtest/gtest.h>
#include <postgres/Client.h>
#include <postgres/Error.h>
#include <postgres/Field.h>
#include "Timestamps.h"

namespace postgres {

TEST(TestField, Meta) {
    auto const res = Client{}.exec("SELECT 'foo'::TEXT AS bar");
    auto const fld = res[0][0];
    ASSERT_FALSE(fld.isNull());
    ASSERT_STREQ("bar", fld.name());
    ASSERT_STREQ("foo", fld.value());
    ASSERT_EQ(TEXTOID, fld.type());
    ASSERT_EQ(3, fld.length());
    ASSERT_EQ(1, fld.format());
}

TEST(TestField, MetaNull) {
    auto const res = Client{}.exec("SELECT NULL");
    auto const fld = res[0][0];
    ASSERT_TRUE(fld.isNull());
}

TEST(TestField, Arithm) {
    ASSERT_EQ(true, Client{}.exec("SELECT TRUE")[0][0].as<bool>());
    ASSERT_EQ(false, Client{}.exec("SELECT FALSE")[0][0].as<bool>());
    ASSERT_EQ(1, Client{}.exec("SELECT 1::SMALLINT")[0][0].as<int16_t>());
    ASSERT_EQ(2, Client{}.exec("SELECT 2::INT")[0][0].as<int32_t>());
    ASSERT_EQ(3, Client{}.exec("SELECT 3::BIGINT")[0][0].as<int64_t>());
    ASSERT_NEAR(4.56, Client{}.exec("SELECT 4.56::REAL")[0][0].as<float>(), 0.001);
    ASSERT_NEAR(5.67, Client{}.exec("SELECT 5.67::DOUBLE PRECISION")[0][0].as<double>(), 0.001);
}

TEST(TestField, ArithmLarge) {
    ASSERT_EQ(1, Client{}.exec("SELECT 1::SMALLINT")[0][0].as<int32_t>());
    ASSERT_EQ(2, Client{}.exec("SELECT 2::INT")[0][0].as<int64_t>());
    ASSERT_NEAR(4.56, Client{}.exec("SELECT 4.56::REAL")[0][0].as<double>(), 0.001);
}

TEST(TestField, ArithmNarrow) {
    ASSERT_THROW(Client{}.exec("SELECT 1::SMALLINT")[0][0].as<int8_t>(), Error);
    ASSERT_THROW(Client{}.exec("SELECT 2::INT")[0][0].as<int16_t>(), Error);
    ASSERT_THROW(Client{}.exec("SELECT 3::BIGINT")[0][0].as<int32_t>(), Error);
    ASSERT_THROW(Client{}.exec("SELECT 4.56::DOUBLE PRECISION")[0][0].as<float>(), Error);
}

TEST(TestField, ArithmUnsigned) {
    ASSERT_EQ(1, Client{}.exec("SELECT 1::SMALLINT")[0][0].as<uint16_t>());
    ASSERT_EQ(2, Client{}.exec("SELECT 2::INT")[0][0].as<uint32_t>());
    ASSERT_EQ(3, Client{}.exec("SELECT 3::BIGINT")[0][0].as<uint64_t>());
    ASSERT_THROW(Client{}.exec("SELECT -1::SMALLINT")[0][0].as<uint16_t>(), Error);
    ASSERT_THROW(Client{}.exec("SELECT -2::INT")[0][0].as<uint32_t>(), Error);
    ASSERT_THROW(Client{}.exec("SELECT -3::BIGINT")[0][0].as<uint64_t>(), Error);
}

TEST(TestField, ArithmRound) {
    ASSERT_THROW(Client{}.exec("SELECT 3::BIGINT")[0][0].as<double>(), Error);
    ASSERT_THROW(Client{}.exec("SELECT 4.56::DOUBLE PRECISION")[0][0].as<int64_t>(), Error);
}

TEST(TestField, ArithmUnsupport) {
    ASSERT_THROW(Client{}.exec("SELECT 3::DECIMAL")[0][0].as<long double>(), Error);
    ASSERT_THROW(Client{}.exec("SELECT 4::NUMERIC")[0][0].as<long double>(), Error);
}

TEST(TestField, Str) {
    ASSERT_EQ("foo", Client{}.exec("SELECT 'foo'")[0][0].as<std::string>());
}

TEST(TestField, Time) {
    auto const res = Client{}.exec("SELECT '2017-08-25 13:03:35'::TIMESTAMP");
    auto const fld = res[0][0];
    ASSERT_EQ(timeSample(), fld.as<Time>().toUnix());
    ASSERT_EQ(timePointSample(), fld.as<Time::Point>());
}

TEST(TestField, TimeBad) {
    ASSERT_THROW(Client{}.exec("SELECT '2017-08-25 13:03:35'")[0][0].as<Time>().toUnix(), Error);
}

TEST(TestField, Assign) {
    int32_t val = 0;
    Client{}.exec("SELECT 1::INT")[0][0] >> val;
    ASSERT_EQ(1, val);
}

TEST(TestField, AssignNull) {
    int32_t val = 0;
    ASSERT_THROW(Client{}.exec("SELECT NULL")[0][0] >> val, Error);
}

TEST(TestField, Ptr) {
    int32_t val = 0;
    auto    ptr = &val;
    Client{}.exec("SELECT 1::INT")[0][0] >> ptr;
    ASSERT_NE(nullptr, ptr);
    ASSERT_EQ(1, *ptr);
}

TEST(TestField, PtrNull) {
    int32_t val = 0;
    auto    ptr = &val;
    Client{}.exec("SELECT NULL")[0][0] >> ptr;
    ASSERT_EQ(nullptr, ptr);
}

TEST(TestField, Opt) {
    std::optional<int32_t> opt{};
    Client{}.exec("SELECT 1::INT")[0][0] >> opt;
    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(1, opt.value());
}

TEST(TestField, OptNull) {
    std::optional<int32_t> opt;
    Client{}.exec("SELECT NULL")[0][0] >> opt;
    ASSERT_FALSE(opt.has_value());
}

TEST(TestField, OptAs) {
    auto const opt = Client{}.exec("SELECT 1::INT")[0][0].as<std::optional<int32_t>>();
    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(1, opt.value());
}

TEST(TestField, OptAsNull) {
    auto const opt = Client{}.exec("SELECT NULL")[0][0].as<std::optional<int32_t>>();
    ASSERT_FALSE(opt.has_value());
}

}  // namespace postgres
