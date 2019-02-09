#include <utility>
#include <regex>
#include <postgres/Timestamp.h>
#include <postgres/internal/Assert.h>

namespace postgres {

Timestamp::Timestamp(const time_t timestamp, const bool has_timezone)
    : Timestamp{std::chrono::system_clock::from_time_t(timestamp), has_timezone}
{}

Timestamp::Timestamp(
    const std::chrono::system_clock::duration dur,
    const std::chrono::system_clock::time_point epoch,
    const bool has_timezone)
    : Timestamp{epoch + dur, has_timezone}
{}

Timestamp::Timestamp(const std::chrono::system_clock::time_point time, const bool has_timezone)
    : time_{time}, has_timezone_{has_timezone}
{}

Timestamp::Timestamp(const std::string& formatted) : has_timezone_{false} {
    // 2017-08-25T13:03:35
    // 2017-08-25T13:03:35.123456789
    // 2017-08-25 13:03:35
    // 2017-08-25 13:03:35.123457
    static const std::regex expr{
        "([[:digit:]]{4})-"  // year
        "([[:digit:]]{2})-"  // month
        "([[:digit:]]{2})"  // day
        "[[:space:]T]"
        "([[:digit:]]{2}):"  // hours
        "([[:digit:]]{2}):"  // minutes
        "([[:digit:]]{2})"  // seconds
        "\\.?"
        "([[:digit:]]{1,9})?"  // fractional seconds
    };

    std::smatch match{};
    _POSTGRES_CXX_ASSERT(
        std::regex_match(formatted, match, expr),
        "Unexpected timestamp format " << formatted);

    auto const years = std::stol(match[1].str());
    auto const months = std::stol(match[2].str());
    auto days = std::stol(match[3].str());
    auto hours = std::stol(match[4].str());
    auto minutes = std::stol(match[5].str());
    auto seconds = std::stol(match[6].str());
    auto fraction = match[7].str();

    _POSTGRES_CXX_ASSERT(1 <= months && months <= 12, "Invalid month " << months);
    _POSTGRES_CXX_ASSERT(1 <= days && days <= 31, "Invalid month day " << days);
    _POSTGRES_CXX_ASSERT(0 <= hours && hours <= 23, "Invalid hour " << hours);
    _POSTGRES_CXX_ASSERT(0 <= minutes && minutes <= 59, "Invalid minute " << minutes);
    _POSTGRES_CXX_ASSERT(0 <= seconds && seconds <= 59, "Invalid minute " << seconds);

    static const int months_len[] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static auto const isLeap = [](const int year) {
        // There is a leap year every year divisible by four
        // except for years which are both divisible by 100 and not divisible by 400.
        return !(year % 4) && ((year % 100) || !(year % 400));
    };

    for (auto year = years; year < 1970; ++year) {
        days -= isLeap(year) ? 366 : 365;
    }
    for (auto year = 1970; year < years; ++year) {
        days += isLeap(year) ? 366 : 365;
    }
    for (auto month = 1; month < months; ++month) {
        days += months_len[month - 1];
    }
    if (months > 2 && isLeap(years)) {
        ++days;
    }
    hours += (days - 1) * 24;
    minutes += hours * 60;
    seconds += minutes * 60;

    time_ = std::chrono::system_clock::from_time_t(seconds);
    if (!fraction.empty()) {
        fraction.resize(9, '0');
        time_ += std::chrono::nanoseconds{std::stol(fraction)};
    }
}

Timestamp::Timestamp(const Timestamp& other) = default;

Timestamp::Timestamp(Timestamp&& other) = default;

Timestamp& Timestamp::operator=(const Timestamp& other) = default;

Timestamp& Timestamp::operator=(Timestamp&& other) = default;

Timestamp::~Timestamp() = default;

time_t Timestamp::unixTime() const {
    return std::chrono::system_clock::to_time_t(time_);
}

time_t Timestamp::postgresTime() const {
    return std::chrono::duration_cast<std::chrono::microseconds>(time_ - postgresEpoch()).count();
}

std::chrono::system_clock::time_point Timestamp::timePoint() const {
    return time_;
}

std::string Timestamp::format() const {
    std::string res(64, 0);
    auto const duration = time_.time_since_epoch();
    auto const seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto const nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);

    auto time = unixTime();
    tm parts{};
    res.resize(
        has_timezone_ ?
        strftime(
            &res[0],
            res.size(),
            nanos.count() ? "%FT%T.000000000 %z" : "%FT%T %z",
            localtime_r(&time, &parts)) :
        strftime(
            &res[0],
            res.size(),
            nanos.count() ? "%FT%T.000000000" : "%FT%T",
            gmtime_r(&time, &parts)));

    //  00000000001111111111222222222233333
    //  01234567890123456789012345678901234
    //  YYYY-mm-ddTHH:MM:SS.000000000 +0300
    if (nanos.count()) {
        res.replace(20, 9, std::to_string(nanos.count()));
    }

    return res;
}

bool Timestamp::hasTimezone() const {
    return has_timezone_;
}

}  // namespace postgres
