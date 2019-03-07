#include <postgres/Time.h>
#include <regex>
#include <utility>
#include <postgres/Error.h>

namespace postgres {

using std::chrono::seconds;
using std::chrono::microseconds;
using std::chrono::nanoseconds;

inline bool isLeap(long const year) {
    // There is a leap year every year divisible by four
    // except for years which are both divisible by 100 and not divisible by 400.
    return !(year % 4) && ((year % 100) || !(year % 400));
}

Time::Time() = default;

Time::Time(time_t const uni, bool const has_zone)
    : Time{Clock::from_time_t(uni), has_zone} {
}

Time::Time(Point const pnt, bool const has_zone)
    : pnt_{pnt}, has_zone_{has_zone} {
}

Time::Time(std::string const& s) {
    // 2017-08-25T13:03:35
    // 2017-08-25T13:03:35.123456789
    // 2017-08-25 13:03:35
    // 2017-08-25 13:03:35.123457
    static std::regex const expr{"([[:digit:]]{4})-"    // years
                                 "([[:digit:]]{2})-"    // months
                                 "([[:digit:]]{2})"     // days
                                 "[[:space:]T]"         // delimiter
                                 "([[:digit:]]{2}):"    // hours
                                 "([[:digit:]]{2}):"    // minutes
                                 "([[:digit:]]{2})"     // seconds
                                 "\\.?"                 // dot
                                 "([[:digit:]]{1,9})?"  // fraction
    };

    std::smatch match{};
    _POSTGRES_CXX_ASSERT(LogicError,
                         std::regex_match(s, match, expr),
                         "bad time format: '" << s << "'");

    auto const years    = std::stol(match[1].str());
    auto const months   = std::stol(match[2].str());
    auto       days     = std::stol(match[3].str());
    auto       hours    = std::stol(match[4].str());
    auto       minutes  = std::stol(match[5].str());
    auto       seconds  = std::stol(match[6].str());
    auto       fraction = match[7].str();

    _POSTGRES_CXX_ASSERT(LogicError, 1 <= months && months <= 12, "bad month in '" << s << "'");
    _POSTGRES_CXX_ASSERT(LogicError, 1 <= days && days <= 31, "bad month day in '" << s << "'");
    _POSTGRES_CXX_ASSERT(LogicError, 0 <= hours && hours <= 23, "bad hours in '" << s << "'");
    _POSTGRES_CXX_ASSERT(LogicError, 0 <= minutes && minutes <= 59, "bad minutes in '" << s << "'");
    _POSTGRES_CXX_ASSERT(LogicError, 0 <= seconds && seconds <= 59, "bad seconds in '" << s << "'");

    for (auto y = years; y < 1970; ++y) {
        days -= isLeap(y) ? 366 : 365;
    }
    for (auto y = 1970; y < years; ++y) {
        days += isLeap(y) ? 366 : 365;
    }
    for (auto m = 1; m < months; ++m) {
        days += ((int[]) {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31})[m - 1];
    }
    if (2 < months && isLeap(years)) {
        ++days;
    }
    hours += (days - 1) * 24;
    minutes += hours * 60;
    seconds += minutes * 60;

    pnt_ = Clock::from_time_t(seconds);
    if (!fraction.empty()) {
        fraction.resize(9, '0');
        pnt_ += nanoseconds{std::stol(fraction)};
    }
}

Time::Time(Time const& other) noexcept = default;

Time& Time::operator=(Time const& other) noexcept = default;

Time::Time(Time&& other) noexcept = default;

Time& Time::operator=(Time&& other) noexcept = default;

Time::~Time() noexcept = default;

time_t Time::toUnix() const {
    return Clock::to_time_t(pnt_);
}

time_t Time::toPostgres() const {
    return std::chrono::duration_cast<microseconds>(pnt_ - EPOCH).count();
}

std::string Time::toString() const {
    std::string res(64, 0);
    auto const  dur   = pnt_.time_since_epoch();
    auto const  secs  = std::chrono::duration_cast<seconds>(dur);
    auto const  nanos = std::chrono::duration_cast<nanoseconds>(dur - secs);
    auto        uni   = toUnix();
    tm          parts{};

    if (has_zone_) {
        res.resize(strftime(&res[0],
                            res.size(),
                            nanos.count() ? "%FT%T.000000000 %z" : "%FT%T %z",
                            localtime_r(&uni, &parts)));
    } else {
        res.resize(strftime(&res[0],
                            res.size(),
                            nanos.count() ? "%FT%T.000000000" : "%FT%T",
                            gmtime_r(&uni, &parts)));
    }

    //  00000000001111111111222222222233333
    //  01234567890123456789012345678901234
    //  YYYY-mm-ddTHH:MM:SS.000000000 +0300
    if (nanos.count()) {
        res.replace(20, 9, std::to_string(nanos.count()));
    }
    return res;
}

Time::Point Time::point() const {
    return pnt_;
}

bool Time::hasZone() const {
    return has_zone_;
}

}  // namespace postgres
