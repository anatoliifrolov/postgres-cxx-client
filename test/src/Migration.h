#pragma once

#include <string>
#include <chrono>
#include <postgres/Config.h>
#include <postgres/Client.h>

namespace postgres {

postgres::Config getConfig();

struct test {
    template <typename V>
    static void visit_static(V& visitor) {
        visitor.template apply<test>("int2", &test::int2);
        visitor.template apply<test>("int4", &test::int4);
        visitor.template apply<test>("int8", &test::int8);
        visitor.template apply<test>("float4", &test::float4);
        visitor.template apply<test>("float8", &test::float8);
        visitor.template apply<test>("flag", &test::flag);
        visitor.template apply<test>("info", &test::info);
        visitor.template apply<test>("time", &test::time);
    }

    template <typename V>
    void visit(V& visitor) {
        visitor.template apply<test>("int2", &test::int2, &int2);
        visitor.template apply<test>("int4", &test::int4, &int4);
        visitor.template apply<test>("int8", &test::int8, &int8);
        visitor.template apply<test>("float4", &test::float4, &float4);
        visitor.template apply<test>("float8", &test::float8, &float8);
        visitor.template apply<test>("flag", &test::flag, &flag);
        visitor.template apply<test>("info", &test::info, &info);
        visitor.template apply<test>("time", &test::time, &time);
    }

    template <typename V>
    void visit(V& visitor) const {
        visitor.template apply<test>("int2", &test::int2, &int2);
        visitor.template apply<test>("int4", &test::int4, &int4);
        visitor.template apply<test>("int8", &test::int8, &int8);
        visitor.template apply<test>("float4", &test::float4, &float4);
        visitor.template apply<test>("float8", &test::float8, &float8);
        visitor.template apply<test>("flag", &test::flag, &flag);
        visitor.template apply<test>("info", &test::info, &info);
        visitor.template apply<test>("time", &test::time, &time);
    }

    static constexpr const char* db_table_name = "test";
    static constexpr bool k_has_serialiser = true;

    int16_t int2 = 0;
    int32_t int4 = 0;
    int64_t int8 = 0;
    float float4 = 0;
    double float8 = 0;
    bool flag = false;
    std::string info;
    std::chrono::system_clock::time_point time;
};

struct Migration {
    Migration();

    Client client_;
    Connection* conn_;
};

}  // namespace postgres
