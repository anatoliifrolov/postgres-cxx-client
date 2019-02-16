#pragma once

#include <string>
#include <chrono>
#include <postgres/Config.h>
#include <postgres/Client.h>
#include <postgres/Visitable.h>

namespace postgres {

struct Table {
    int16_t                               int2   = 0;
    int32_t                               int4   = 0;
    int64_t                               int8   = 0;
    float                                 float4 = 0;
    double                                float8 = 0;
    bool                                  flag   = false;
    std::string                           info;
    std::chrono::system_clock::time_point time;

    POSTGRES_CXX_TABLE("test", int2, int4, int8, float4, float8, flag, info, time)
};

struct Migration {
    Migration();

    Client client_;
    Connection* conn_;

private:
    void migrate();
};

}  // namespace postgres
