#pragma once

#include <chrono>
#include <string>

namespace postgres {

class Time {
public:
    using Clock = std::chrono::system_clock;
    using Point = Clock::time_point;

    explicit Time(time_t uni, bool has_zone = false);
    explicit Time(Point pnt, bool has_zone = false);
    explicit Time(std::string const& s);
    Time(Time const& other) noexcept;
    Time& operator=(Time const& other) noexcept;
    Time(Time&& other) noexcept;
    Time& operator=(Time&& other) noexcept;
    ~Time();

    time_t toUnix() const;
    time_t toPostgres() const;
    std::string toString() const;
    Point point() const;
    bool hasZone() const;

    // 2000-01-01 00:00:00
    static auto constexpr EPOCH = Point{std::chrono::seconds{946684800}};

private:
    Point pnt_;
    bool  has_zone_;
};

}  // namespace postgres
