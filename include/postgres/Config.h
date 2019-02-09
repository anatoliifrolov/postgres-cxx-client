#pragma once

#include <string>
#include <list>
#include <vector>
#include <memory>

namespace postgres {

class Connection;

class Config {
public:
    friend class Connection;
    class Builder;

    explicit Config();
    Config(const Config&);
    Config(Config&&);
    Config& operator=(const Config&);
    Config& operator=(Config&&);
    ~Config();

    static Builder init();
    const char* get(const char* const key) const;

private:
    void set(const char* const key, const std::string& val);
    void set(const char* const key, const char* const val);

    template <typename T>
    void set(const char* const key, const T& val) {
        set(key, std::to_string(val));
    }

    const char* const* keywords() const;
    const char* const* values() const;

    std::list<std::shared_ptr<std::string>> storage_;
    std::vector<const char*> keywords_;
    std::vector<const char*> values_;
};

class Config::Builder {
public:
    Builder();
    Builder(Builder const& other);
    Builder(Builder&& other);
    Builder& operator=(Builder const& other);
    Builder& operator=(Builder&& other);
    ~Builder();

    Builder& host(std::string const& val);
    Builder& hostaddr(std::string const& val);
    Builder& port(int const val);
    Builder& user(std::string const& val);
    Builder& password(std::string const& val);
    Builder& dbname(std::string const& val);

    template <typename T>
    Builder& set(char const* const key, T&& val);

    Config build();

private:
    Config cfg_;
};

template <typename T>
Config::Builder& Config::Builder::set(char const* const key, T&& val) {
    cfg_.set(key, std::forward<T>(val));
    return *this;
}

}  // namespace postgres
