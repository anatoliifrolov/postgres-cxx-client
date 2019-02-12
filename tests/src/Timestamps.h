#pragma once

#include <chrono>
#include <ctime>
#include <string>

namespace postgres {

inline constexpr time_t timeSample() {
    return 1503666215;
}

inline constexpr time_t timeSamplePg() {
    return (timeSample() - 946684800) * 1000000;
}

inline constexpr time_t timeSamplePgMicro() {
    return timeSamplePg() + 987654;
}

inline constexpr std::chrono::system_clock::time_point timePointSample() {
    return std::chrono::system_clock::time_point{std::chrono::seconds{timeSample()}};
}

inline constexpr std::chrono::system_clock::time_point timePointSampleMicro() {
    return timePointSample() + std::chrono::microseconds{987654};
}

inline constexpr std::chrono::system_clock::time_point timePointSampleNano() {
    return timePointSample() + std::chrono::nanoseconds{987654321};
}

inline std::string timeFormatSample() {
    return "2017-08-25T13:03:35";
}

inline std::string timeFormatSampleNano() {
    return "2017-08-25T13:03:35.987654321";
}

inline std::string timeFormatSampleNanoTz() {
    return "2017-08-25T16:03:35.987654321 +0300";
}

}  // namespace postgres
