#include <regex>
#include <postgres/Config.h>

namespace postgres {

static bool looksLikeIp(const std::string& host) {
    static const std::regex expr{R"(^[0-9]{1,3}(\.[0-9]{1,3}){3}$)"};
    return std::regex_match(host, expr);
}

Config::Config(
    const std::string& dbname,
    const std::string& user,
    const std::string& password,
    const std::string& host,
    const int port)
{
    keywords_.push_back(nullptr);
    values_.push_back(nullptr);
    if (!dbname.empty()) {
        set("dbname", dbname);
    }
    if (!user.empty()) {
        set("user", user);
    }
    if (!password.empty()) {
        set("password", password);
    }
    if (!host.empty()) {
        set(looksLikeIp(host) ? "hostaddr" : "host", host);
    }
    set("port", port);
}

Config::Config(const Config& other) = default;

Config::Config(Config&& other) = default;

Config& Config::operator=(const Config& other) = default;

Config& Config::operator=(Config&& other) = default;

Config::~Config() = default;

const char* Config::get(const char* const key) const {
    for (auto i = 0u; i < keywords_.size() - 1; ++i) {
        if (strcmp(key, keywords_[i]) == 0) {
            return values_[i];
        }
    }
    return nullptr;
}

void Config::set(const char* const key, const std::string& val) {
    set(key, val.c_str());
}

void Config::set(const char* const key, const char* const val) {
    keywords_.push_back(nullptr);
    values_.push_back(nullptr);
    storage_.push_back(std::make_shared<std::string>(val));
    keywords_[keywords_.size() - 2] = key;
    values_[values_.size() - 2] = storage_.back()->c_str();
}

const char* const* Config::keywords() const {
    return keywords_.data();
}

const char* const* Config::values() const {
    return values_.data();
}

int Config::expandDbname() const {
    return 0;
}

}  // namespace postgres
