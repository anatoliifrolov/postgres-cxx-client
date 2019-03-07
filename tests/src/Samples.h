#pragma once

#include <chrono>
#include <string>

namespace postgres {

inline constexpr auto CONNECT_STR = "user=cxx_client password=cxx_client dbname=cxx_client";
inline constexpr auto CONNECT_URI = "postgresql://cxx_client:cxx_client@/cxx_client";

inline constexpr time_t TIME_SAMPLE          = 1503666215;
inline constexpr time_t TIME_SAMPLE_PG       = (TIME_SAMPLE - 946684800) * 1000000;
inline constexpr time_t TIME_SAMPLE_PG_MICRO = TIME_SAMPLE_PG + 987654;

inline constexpr auto TIME_POINT_SAMPLE       = std::chrono::system_clock::time_point{std::chrono::seconds{
    TIME_SAMPLE}};
inline constexpr auto TIME_POINT_SAMPLE_MICRO = TIME_POINT_SAMPLE
                                                + std::chrono::microseconds{987654};
inline constexpr auto TIME_POINT_SAMPLE_NANO  = TIME_POINT_SAMPLE
                                                + std::chrono::nanoseconds{987654321};

inline std::string const TIME_STR_SAMPLE         = "2017-08-25T13:03:35";
inline std::string const TIME_STR_SAMPLE_NANO    = TIME_STR_SAMPLE + ".987654321";
inline std::string const TIME_STR_SAMPLE_NANO_TZ = "2017-08-25T16:03:35.987654321 +0300";

}  // namespace postgres
