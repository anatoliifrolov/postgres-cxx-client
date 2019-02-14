#include <vector>
#include <gtest/gtest.h>
#include <postgres/internal/Bytes.h>
#include <postgres/Command.h>
#include <postgres/Visitable.h>
#include "Timestamps.h"

namespace postgres {

TEST(TestCommand, Stmt) {
    auto const    stmt = "STMT";
    Command const cmd{stmt};
    ASSERT_EQ(stmt, cmd.statement());
}

TEST(TestCommand, StmtView) {
    std::string const      stmt = "STMT";
    std::string_view const view = stmt;
    Command const          cmd{view};
    ASSERT_EQ(stmt.data(), cmd.statement());
}

TEST(TestCommand, StmtRef) {
    std::string const stmt = "STMT";
    Command const     cmd{stmt};
    ASSERT_EQ(stmt.data(), cmd.statement());
}

TEST(TestCommand, StmtOwn) {
    Command const cmd{std::string{"STMT"}};
    ASSERT_STREQ("STMT", cmd.statement());
}

TEST(TestCommand, NoArgs) {
    Command const cmd{"STMT"};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(0, cmd.count());
    ASSERT_EQ(nullptr, cmd.types());
    ASSERT_EQ(nullptr, cmd.values());
    ASSERT_EQ(nullptr, cmd.lengths());
    ASSERT_EQ(nullptr, cmd.formats());
}

TEST(TestCommand, True) {
    Command const cmd{"STMT", true};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(Oid{BOOLOID}, cmd.types()[0]);
    ASSERT_EQ(true, internal::orderBytes<bool>(cmd.values()[0]));
    ASSERT_EQ(1, cmd.lengths()[0]);
    ASSERT_EQ(1, cmd.formats()[0]);
}

TEST(TestCommand, False) {
    Command const cmd{"STMT", false};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(Oid{BOOLOID}, cmd.types()[0]);
    ASSERT_EQ(false, internal::orderBytes<bool>(cmd.values()[0]));
    ASSERT_EQ(1, cmd.lengths()[0]);
    ASSERT_EQ(1, cmd.formats()[0]);
}

TEST(TestCommand, Int2) {
    Command const cmd{"STMT", int16_t{3}};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(Oid{INT2OID}, cmd.types()[0]);
    ASSERT_EQ(3, internal::orderBytes<int16_t>(cmd.values()[0]));
    ASSERT_EQ(2, cmd.lengths()[0]);
    ASSERT_EQ(1, cmd.formats()[0]);
}

TEST(TestCommand, Int4) {
    Command const cmd{"STMT", int32_t{3}};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(Oid{INT4OID}, cmd.types()[0]);
    ASSERT_EQ(3, internal::orderBytes<int32_t>(cmd.values()[0]));
    ASSERT_EQ(4, cmd.lengths()[0]);
    ASSERT_EQ(1, cmd.formats()[0]);
}

TEST(TestCommand, Int8) {
    Command const cmd{"STMT", int64_t{3}};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(Oid{INT8OID}, cmd.types()[0]);
    ASSERT_EQ(3, internal::orderBytes<int64_t>(cmd.values()[0]));
    ASSERT_EQ(8, cmd.lengths()[0]);
    ASSERT_EQ(1, cmd.formats()[0]);
}

TEST(TestCommand, Float4) {
    Command const cmd{"STMT", float{3.45}};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(Oid{FLOAT4OID}, cmd.types()[0]);
    ASSERT_NEAR(3.45, internal::orderBytes<float>(cmd.values()[0]), 0.001);
    ASSERT_EQ(4, cmd.lengths()[0]);
    ASSERT_EQ(1, cmd.formats()[0]);
}

TEST(TestCommand, Float8) {
    Command const cmd{"STMT", double{3.45}};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(Oid{FLOAT8OID}, cmd.types()[0]);
    ASSERT_NEAR(3.45, internal::orderBytes<double>(cmd.values()[0]), 0.001);
    ASSERT_EQ(8, cmd.lengths()[0]);
    ASSERT_EQ(1, cmd.formats()[0]);
}

TEST(TestCommand, Ptr) {
    int32_t const val = 3;
    auto const    ptr = &val;
    Command const cmd{"STMT", ptr};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(Oid{INT4OID}, cmd.types()[0]);
    ASSERT_EQ(3, internal::orderBytes<int32_t>(cmd.values()[0]));
    ASSERT_EQ(4, cmd.lengths()[0]);
    ASSERT_EQ(1, cmd.formats()[0]);
}

TEST(TestCommand, NullPtr) {
    int* const null_ptr = nullptr;
    Command const cmd{"STMT", null_ptr};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(0u, cmd.types()[0]);
    ASSERT_EQ(nullptr, cmd.values()[0]);
    ASSERT_EQ(0, cmd.lengths()[0]);
    ASSERT_EQ(0, cmd.formats()[0]);
}

TEST(TestCommand, Opt) {
    std::optional<int32_t> opt = 3;
    Command const          cmd{"STMT", opt};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(Oid{INT4OID}, cmd.types()[0]);
    ASSERT_EQ(3, internal::orderBytes<int32_t>(cmd.values()[0]));
    ASSERT_EQ(4, cmd.lengths()[0]);
    ASSERT_EQ(1, cmd.formats()[0]);
}

TEST(TestCommand, NullOpt) {
    std::optional<int32_t> opt{};
    Command const          cmd{"STMT", opt};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(0u, cmd.types()[0]);
    ASSERT_EQ(nullptr, cmd.values()[0]);
    ASSERT_EQ(0, cmd.lengths()[0]);
    ASSERT_EQ(0, cmd.formats()[0]);
}

TEST(TestCommand, EmptyStr) {
    Command const cmd{"STMT", ""};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(0u, cmd.types()[0]);
    ASSERT_STREQ("", cmd.values()[0]);
    ASSERT_EQ(0, cmd.lengths()[0]);
    ASSERT_EQ(0, cmd.formats()[0]);
}

TEST(TestCommand, CStr) {
    auto const    str = "STR";
    Command const cmd{"STMT", str};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(0u, cmd.types()[0]);
    ASSERT_EQ(str, cmd.values()[0]);
    ASSERT_EQ(0, cmd.lengths()[0]);
    ASSERT_EQ(0, cmd.formats()[0]);
}

TEST(TestCommand, StrView) {
    std::string const      str  = "STR";
    std::string_view const view = str;
    Command const          cmd{"STMT", view};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(0u, cmd.types()[0]);
    ASSERT_EQ(str.data(), cmd.values()[0]);
    ASSERT_EQ(0, cmd.lengths()[0]);
    ASSERT_EQ(0, cmd.formats()[0]);
}

TEST(TestCommand, StrRef) {
    std::string const str = "STR";
    Command const     cmd{"STMT", str};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(0u, cmd.types()[0]);
    ASSERT_EQ(str.data(), cmd.values()[0]);
    ASSERT_EQ(0, cmd.lengths()[0]);
    ASSERT_EQ(0, cmd.formats()[0]);
}

TEST(TestCommand, StrOwn) {
    Command const cmd{"STMT", std::string{"STR"}};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(0u, cmd.types()[0]);
    ASSERT_STREQ("STR", cmd.values()[0]);
    ASSERT_EQ(4, cmd.lengths()[0]);
    ASSERT_EQ(0, cmd.formats()[0]);
}

TEST(TestCommand, Oid) {
    std::string   str  = "STR";
    auto const    data = str.data();
    Command const cmd{"STMT", bindOid(std::move(str), ANYOID)};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(Oid{ANYOID}, cmd.types()[0]);
    ASSERT_STREQ("STR", cmd.values()[0]);
    ASSERT_NE(data, cmd.values()[0]);
    ASSERT_EQ(4, cmd.lengths()[0]);
    ASSERT_EQ(0, cmd.formats()[0]);
}

TEST(TestCommand, Time) {
    Command const cmd{"STMT", timePointSample()};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(Oid{TIMESTAMPOID}, cmd.types()[0]);
    ASSERT_EQ(timeSamplePg(), internal::orderBytes<time_t>(cmd.values()[0]));
    ASSERT_EQ(static_cast<int>(sizeof(time_t)), cmd.lengths()[0]);
    ASSERT_EQ(1, cmd.formats()[0]);
}

TEST(TestCommand, TimeZone) {
    Command const cmd{"STMT", Time{timePointSampleNano(), true}};
    ASSERT_EQ(1, cmd.count());
    ASSERT_EQ(Oid{TIMESTAMPTZOID}, cmd.types()[0]);
    ASSERT_EQ(timeFormatSampleNanoTz(), cmd.values()[0]);
    ASSERT_EQ(static_cast<int>(timeFormatSampleNanoTz().size() + 1), cmd.lengths()[0]);
    ASSERT_EQ(0, cmd.formats()[0]);
}

TEST(TestCommand, Range) {
    std::vector<int32_t> const arr{1, 2, 3};

    Command const cmd{"STMT", std::make_pair(arr.begin(), arr.end())};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(3, cmd.count());

    ASSERT_EQ(Oid{INT4OID}, cmd.types()[0]);
    ASSERT_EQ(1, internal::orderBytes<int32_t>(cmd.values()[0]));
    ASSERT_EQ(4, cmd.lengths()[0]);
    ASSERT_EQ(1, cmd.formats()[0]);

    ASSERT_EQ(Oid{INT4OID}, cmd.types()[1]);
    ASSERT_EQ(2, internal::orderBytes<int32_t>(cmd.values()[1]));
    ASSERT_EQ(4, cmd.lengths()[1]);
    ASSERT_EQ(1, cmd.formats()[1]);

    ASSERT_EQ(Oid{INT4OID}, cmd.types()[2]);
    ASSERT_EQ(3, internal::orderBytes<int32_t>(cmd.values()[2]));
    ASSERT_EQ(4, cmd.lengths()[2]);
    ASSERT_EQ(1, cmd.formats()[2]);
}

struct Visitable {
    std::string s;
    int32_t     n = 0;
    double      f = 0.0;

    POSTGRES_CXX_TABLE(Visitable, s, n, f);
};

TEST(TestCommand, Visit) {
    Visitable const v{"TEXT", 3, 4.56};
    Command const   cmd{"STMT", v};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(3, cmd.count());

    ASSERT_EQ(0u, cmd.types()[0]);
    ASSERT_EQ(v.s.data(), cmd.values()[0]);
    ASSERT_EQ(0, cmd.lengths()[0]);
    ASSERT_EQ(0, cmd.formats()[0]);

    ASSERT_EQ(Oid{INT4OID}, cmd.types()[1]);
    ASSERT_EQ(3, internal::orderBytes<int32_t>(cmd.values()[1]));
    ASSERT_EQ(4, cmd.lengths()[1]);
    ASSERT_EQ(1, cmd.formats()[1]);

    ASSERT_EQ(Oid{FLOAT8OID}, cmd.types()[2]);
    ASSERT_NEAR(4.56, internal::orderBytes<double>(cmd.values()[2]), 0.001);
    ASSERT_EQ(8, cmd.lengths()[2]);
    ASSERT_EQ(1, cmd.formats()[2]);
}

TEST(TestCommand, MultiArgs) {
    Command const cmd{"STMT", std::string{"TEXT"}, int32_t{3}, 4.56};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(3, cmd.count());

    ASSERT_EQ(0u, cmd.types()[0]);
    ASSERT_STREQ("TEXT", cmd.values()[0]);
    ASSERT_EQ(5, cmd.lengths()[0]);
    ASSERT_EQ(0, cmd.formats()[0]);

    ASSERT_EQ(Oid{INT4OID}, cmd.types()[1]);
    ASSERT_EQ(3, internal::orderBytes<int32_t>(cmd.values()[1]));
    ASSERT_EQ(4, cmd.lengths()[1]);
    ASSERT_EQ(1, cmd.formats()[1]);

    ASSERT_EQ(Oid{FLOAT8OID}, cmd.types()[2]);
    ASSERT_NEAR(4.56, internal::orderBytes<double>(cmd.values()[2]), 0.001);
    ASSERT_EQ(8, cmd.lengths()[2]);
    ASSERT_EQ(1, cmd.formats()[2]);
}

TEST(TestCommand, Dynamic) {
    Command    cmd{"STMT"};
    auto const str = "STR";
    cmd << str << std::string{"STR2"};
    ASSERT_STREQ("STMT", cmd.statement());
    ASSERT_EQ(2, cmd.count());

    ASSERT_EQ(0u, cmd.types()[0]);
    ASSERT_EQ(str, cmd.values()[0]);
    ASSERT_EQ(0, cmd.lengths()[0]);
    ASSERT_EQ(0, cmd.formats()[0]);

    ASSERT_EQ(0u, cmd.types()[1]);
    ASSERT_STREQ("STR2", cmd.values()[1]);
    ASSERT_EQ(5, cmd.lengths()[1]);
    ASSERT_EQ(0, cmd.formats()[1]);
}

}  // namespace postgres
