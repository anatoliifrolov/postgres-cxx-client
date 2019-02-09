#pragma once

#include <chrono>
#include <ctime>
#include <string>

namespace postgres {

inline constexpr std::chrono::system_clock::time_point unixEpoch() {
    return std::chrono::system_clock::time_point{};
}

inline constexpr std::chrono::system_clock::time_point postgresEpoch() {
    // 2000-01-01 00:00:00
    return std::chrono::system_clock::time_point{std::chrono::seconds{946684800}};
}

class Timestamp {
public:
    explicit Timestamp(const time_t timestamp, const bool has_timezone = false);
    explicit Timestamp(const std::chrono::system_clock::duration dur,
                       const std::chrono::system_clock::time_point epoch = unixEpoch(),
                       const bool has_timezone = false);
    explicit Timestamp(const std::chrono::system_clock::time_point time,
                       const bool has_timezone = false);
    explicit Timestamp(const std::string& formatted);
    Timestamp(const Timestamp& other);
    Timestamp(Timestamp&& other);
    Timestamp& operator=(const Timestamp& other);
    Timestamp& operator=(Timestamp&& other);
    ~Timestamp();

    time_t unixTime() const;
    time_t postgresTime() const;
    std::chrono::system_clock::time_point timePoint() const;
    std::string format() const;
    bool hasTimezone() const;

private:
    std::chrono::system_clock::time_point time_;
    bool                                  has_timezone_;
};

template <typename... Ts>
Timestamp makeTimestamp(Ts&& ... init) {
    return Timestamp{std::forward<Ts>(init)...};
}

}  // namespace postgres
