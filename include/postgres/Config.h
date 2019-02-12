#pragma once

#include <map>
#include <string>
#include <vector>

namespace postgres {

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
    explicit Builder();
    Builder(Builder const& other) = delete;
    Builder& operator=(Builder const& other) = delete;
    Builder(Builder&& other) noexcept;
    Builder& operator=(Builder&& other) noexcept;
    ~Builder();

    Builder& host(std::string const& val);
    Builder& hostaddr(std::string const& val);
    Builder& port(int val);
    Builder& user(std::string const& val);
    Builder& password(std::string const& val);
    Builder& dbname(std::string const& val);
    Builder& set(std::string const& key, int val);
    Builder& set(std::string const& key, std::string const& val);
    Config build();

private:
    Config cfg_;
};

}  // namespace postgres
