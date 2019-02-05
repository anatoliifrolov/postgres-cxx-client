#pragma once

#include <string>
#include <chrono>
#include <postgres/Config.h>
#include <postgres/Client.h>

namespace postgres {

postgres::Config getConfig();

struct RowMapping {
    template <typename V>
    static void visit_static(V& visitor) {
        visitor.template apply<RowMapping>("int2", &RowMapping::int2);
        visitor.template apply<RowMapping>("int4", &RowMapping::int4);
        visitor.template apply<RowMapping>("int8", &RowMapping::int8);
        visitor.template apply<RowMapping>("float4", &RowMapping::float4);
        visitor.template apply<RowMapping>("float8", &RowMapping::float8);
        visitor.template apply<RowMapping>("flag", &RowMapping::flag);
        visitor.template apply<RowMapping>("info", &RowMapping::info);
        visitor.template apply<RowMapping>("time", &RowMapping::time);
    }

    template <typename V>
    void visit(V& visitor) {
        visitor.template apply<RowMapping>("int2", &RowMapping::int2, &int2);
        visitor.template apply<RowMapping>("int4", &RowMapping::int4, &int4);
        visitor.template apply<RowMapping>("int8", &RowMapping::int8, &int8);
        visitor.template apply<RowMapping>("float4", &RowMapping::float4, &float4);
        visitor.template apply<RowMapping>("float8", &RowMapping::float8, &float8);
        visitor.template apply<RowMapping>("flag", &RowMapping::flag, &flag);
        visitor.template apply<RowMapping>("info", &RowMapping::info, &info);
        visitor.template apply<RowMapping>("time", &RowMapping::time, &time);
    }

    template <typename V>
    void visit(V& visitor) const {
        visitor.template apply<RowMapping>("int2", &RowMapping::int2, &int2);
        visitor.template apply<RowMapping>("int4", &RowMapping::int4, &int4);
        visitor.template apply<RowMapping>("int8", &RowMapping::int8, &int8);
        visitor.template apply<RowMapping>("float4", &RowMapping::float4, &float4);
        visitor.template apply<RowMapping>("float8", &RowMapping::float8, &float8);
        visitor.template apply<RowMapping>("flag", &RowMapping::flag, &flag);
        visitor.template apply<RowMapping>("info", &RowMapping::info, &info);
        visitor.template apply<RowMapping>("time", &RowMapping::time, &time);
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
