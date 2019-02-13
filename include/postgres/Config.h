#pragma once

#include <chrono>
#include <map>
#include <string>
#include <vector>

namespace postgres {

enum class SslMode {
    ALLOW,
    DISABLE,
    PREFER,
    REQUIRE,
    VERIFY_CA,
    VERIFY_FULL,
    DEFAULT,
};

enum class TargetSession {
    ANY,
    READ_WRITE,
    DEFAULT,
};

class Config {
public:
    class Builder;

    Config(Config const& other) = delete;
    Config& operator=(Config const& other) = delete;
    Config(Config&& other) noexcept;
    Config& operator=(Config&& other) noexcept;
    ~Config();

    static Config make();

    // libpq interface adapters.
    char const* const* keys() const;
    char const* const* values() const;

private:
    explicit Config();

    std::vector<char const*>           keys_;
    std::vector<char const*>           vals_;
    std::map<std::string, std::string> params_;
};

class Config::Builder {
public:
    using Duration = std::chrono::high_resolution_clock::duration;

    explicit Builder();
    Builder(Builder const& other) = delete;
    Builder& operator=(Builder const& other) = delete;
    Builder(Builder&& other) noexcept;
    Builder& operator=(Builder&& other) noexcept;
    ~Builder();

    Builder& application_name(std::string const& val);
    Builder& client_encoding(std::string const& val);
    Builder& connect_timeout(Duration val);
    Builder& dbname(std::string const& val);
    Builder& fallback_application_name(std::string const& val);
    Builder& gsslib(std::string const& val);
    Builder& host(std::string const& val);
    Builder& hostaddr(std::string const& val);
    Builder& keepalives(bool val);
    Builder& keepalives_count(int val);
    Builder& keepalives_idle(Duration val);
    Builder& keepalives_interval(Duration val);
    Builder& krbsrvname(std::string const& val);
    Builder& options(std::string const& val);
    Builder& passfile(std::string const& val);
    Builder& password(std::string const& val);
    Builder& port(int val);
    Builder& requirepeer(std::string const& val);
    Builder& requiressl(bool val);
    Builder& service(std::string const& val);
    Builder& sslcert(std::string const& val);
    Builder& sslcompression(bool val);
    Builder& sslcrl(std::string const& val);
    Builder& sslkey(std::string const& val);
    Builder& sslmode(SslMode val);
    Builder& sslrootcert(std::string const& val);
    Builder& target_session_attrs(TargetSession val);
    Builder& user(std::string const& val);

    Builder& disable(std::string const& key);
    Builder& enable(std::string const& key);
    Builder& setNumber(std::string const& key, int val);
    Builder& setInterval(std::string const& key, Duration val);
    Builder& set(std::string const& key, std::string const& val);

    Config build();

private:
    Config cfg_;
};

}  // namespace postgres
