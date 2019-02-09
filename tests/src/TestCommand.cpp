#include <arpa/inet.h>
#include <functional>
#include <vector>
#include <gtest/gtest.h>
#include <postgres/Command.h>
#include <postgres/Timestamp.h>
#include <postgres/internal/Bytes.h>
#include "TimestampSamples.h"

namespace postgres {

TEST(TestCommand, Dynamic) {
    Command cmd{"STATEMENT"};
    ASSERT_STREQ("STATEMENT", cmd.statement());
    ASSERT_EQ(0, cmd.nParams());
    ASSERT_EQ(nullptr, cmd.paramTypes());
    ASSERT_EQ(nullptr, cmd.paramValues());
    ASSERT_EQ(nullptr, cmd.paramLengths());
    ASSERT_EQ(nullptr, cmd.paramFormats());
    const std::vector<int> v1{4, 5};
    const std::vector<int> v2{6, 7, 8};
    cmd << 1 << 2 << 3 << v1 << std::make_pair(v2.begin(), v2.begin() + 2);
    ASSERT_EQ(7, cmd.nParams());
    for (auto i = 0; i <= 6; ++i) {
        ASSERT_EQ(static_cast<Oid>(INT4OID), cmd.paramTypes()[i]);
        ASSERT_EQ(i + 1, internal::orderBytes<int32_t>(cmd.paramValues()[i]));
        ASSERT_EQ(static_cast<int>(sizeof(int)), cmd.paramLengths()[i]);
        ASSERT_EQ(1, cmd.paramFormats()[i]);
    }
}

TEST(TestCommand, Types) {
    const auto n = int32_t{42};
    const auto valid_ptr = &n;
    const char* null_ptr = nullptr;
    const auto str = "C_STRING";

    const Command cmd{
        "STATEMENT",

        // Arithmetic literals.
        'C',
        int16_t{2},
        int32_t{4},
        int64_t{8},
        float{4.44},
        8.88,

        // Boolean values.
        true,
        false,

        // Pointers.
        nullptr,
        null_ptr,
        valid_ptr,

        // Strings.
        "",
        str,
        std::string{"STRING"},

        // Special types.
        bindOid(n, ANYOID),

        // Timestamps.
        timePointSample(),
        makeTimestamp(timePointSampleNs(), true)};

    ASSERT_STREQ("STATEMENT", cmd.statement());
    ASSERT_EQ(17, cmd.nParams());

    auto i = 0;
    ASSERT_EQ(static_cast<Oid>(BYTEAOID), cmd.paramTypes()[i]);
    ASSERT_EQ('C', *cmd.paramValues()[i]);
    ASSERT_EQ(static_cast<int>(sizeof('C')), cmd.paramLengths()[i]);
    ASSERT_EQ(1, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(static_cast<Oid>(INT2OID), cmd.paramTypes()[i]);
    ASSERT_EQ(2, internal::orderBytes<int16_t>(cmd.paramValues()[i]));
    ASSERT_EQ(static_cast<int>(sizeof(int16_t)), cmd.paramLengths()[i]);
    ASSERT_EQ(1, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(static_cast<Oid>(INT4OID), cmd.paramTypes()[i]);
    ASSERT_EQ(4, internal::orderBytes<int32_t>(cmd.paramValues()[i]));
    ASSERT_EQ(static_cast<int>(sizeof(int32_t)), cmd.paramLengths()[i]);
    ASSERT_EQ(1, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(static_cast<Oid>(INT8OID), cmd.paramTypes()[i]);
    ASSERT_EQ(8, internal::orderBytes<int64_t>(cmd.paramValues()[i]));
    ASSERT_EQ(static_cast<int>(sizeof(int64_t)), cmd.paramLengths()[i]);
    ASSERT_EQ(1, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(static_cast<Oid>(FLOAT4OID), cmd.paramTypes()[i]);
    ASSERT_NEAR(4.44, internal::orderBytes<float>(cmd.paramValues()[i]), 0.001);
    ASSERT_EQ(static_cast<int>(sizeof(float)), cmd.paramLengths()[i]);
    ASSERT_EQ(1, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(static_cast<Oid>(FLOAT8OID), cmd.paramTypes()[i]);
    ASSERT_NEAR(8.88, internal::orderBytes<double>(cmd.paramValues()[i]), 0.001);
    ASSERT_EQ(static_cast<int>(sizeof(double)), cmd.paramLengths()[i]);
    ASSERT_EQ(1, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(static_cast<Oid>(BOOLOID), cmd.paramTypes()[i]);
    ASSERT_TRUE(*reinterpret_cast<const bool*>(cmd.paramValues()[i]));
    ASSERT_EQ(1, cmd.paramLengths()[i]);
    ASSERT_EQ(1, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(static_cast<Oid>(BOOLOID), cmd.paramTypes()[i]);
    ASSERT_FALSE(*reinterpret_cast<const bool*>(cmd.paramValues()[i]));
    ASSERT_EQ(1, cmd.paramLengths()[i]);
    ASSERT_EQ(1, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(0u, cmd.paramTypes()[i]);
    ASSERT_EQ(nullptr, cmd.paramValues()[i]);
    ASSERT_EQ(0, cmd.paramLengths()[i]);
    ASSERT_EQ(0, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(0u, cmd.paramTypes()[i]);
    ASSERT_EQ(nullptr, cmd.paramValues()[i]);
    ASSERT_EQ(0, cmd.paramLengths()[i]);
    ASSERT_EQ(0, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(static_cast<Oid>(INT4OID), cmd.paramTypes()[i]);
    ASSERT_EQ(42, internal::orderBytes<int32_t>(cmd.paramValues()[i]));
    ASSERT_EQ(static_cast<int>(sizeof(int32_t)), cmd.paramLengths()[i]);
    ASSERT_EQ(1, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(0u, cmd.paramTypes()[i]);
    ASSERT_STREQ("", cmd.paramValues()[i]);
    ASSERT_EQ(0, cmd.paramLengths()[i]);
    ASSERT_EQ(0, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(0u, cmd.paramTypes()[i]);
    ASSERT_EQ(str, cmd.paramValues()[i]);
    ASSERT_EQ(0, cmd.paramLengths()[i]);
    ASSERT_EQ(0, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(0u, cmd.paramTypes()[i]);
    ASSERT_STREQ("STRING", cmd.paramValues()[i]);
    ASSERT_EQ(static_cast<int>(sizeof("STRING")), cmd.paramLengths()[i]);
    ASSERT_EQ(0, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(static_cast<Oid>(ANYOID), cmd.paramTypes()[i]);
    ASSERT_EQ(42, internal::orderBytes<int32_t>(cmd.paramValues()[i]));
    ASSERT_EQ(static_cast<int>(sizeof(int32_t)), cmd.paramLengths()[i]);
    ASSERT_EQ(1, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(static_cast<Oid>(TIMESTAMPOID), cmd.paramTypes()[i]);
    ASSERT_EQ(timeSamplePg(), internal::orderBytes<time_t>(cmd.paramValues()[i]));
    ASSERT_EQ(static_cast<int>(sizeof(time_t)), cmd.paramLengths()[i]);
    ASSERT_EQ(1, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(static_cast<Oid>(TIMESTAMPTZOID), cmd.paramTypes()[i]);
    ASSERT_EQ(timeSampleFormatPreciseTz(), cmd.paramValues()[i]);
    ASSERT_EQ(static_cast<int>(timeSampleFormatPreciseTz().size() + 1), cmd.paramLengths()[i]);
    ASSERT_EQ(0, cmd.paramFormats()[i]);
}

TEST(TestCommand, Iterators) {
    std::vector<int32_t> vals{1, 2, 3};
    const Command cmd{"STATEMENT", vals.begin(), vals.end()};
    ASSERT_EQ(3, cmd.nParams());
    auto i = 0;
    ASSERT_EQ(static_cast<Oid>(INT4OID), cmd.paramTypes()[i]);
    ASSERT_EQ(1, internal::orderBytes<int32_t>(cmd.paramValues()[i]));
    ASSERT_EQ(static_cast<int>(sizeof(int32_t)), cmd.paramLengths()[i]);
    ASSERT_EQ(1, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(static_cast<Oid>(INT4OID), cmd.paramTypes()[i]);
    ASSERT_EQ(2, internal::orderBytes<int32_t>(cmd.paramValues()[i]));
    ASSERT_EQ(static_cast<int>(sizeof(int32_t)), cmd.paramLengths()[i]);
    ASSERT_EQ(1, cmd.paramFormats()[i]);
    ++i;
    ASSERT_EQ(static_cast<Oid>(INT4OID), cmd.paramTypes()[i]);
    ASSERT_EQ(3, internal::orderBytes<int32_t>(cmd.paramValues()[i]));
    ASSERT_EQ(static_cast<int>(sizeof(int32_t)), cmd.paramLengths()[i]);
    ASSERT_EQ(1, cmd.paramFormats()[i]);
}

}  // namespace postgres
