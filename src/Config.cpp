#include <cstring>
#include <postgres/Config.h>

namespace postgres {

Config::Config() {
    keywords_.push_back(nullptr);
    values_.push_back(nullptr);
}

Config::Config(Config const& other) = default;

Config::Config(Config&& other) = default;

Config& Config::operator=(Config const& other) = default;

Config& Config::operator=(Config&& other) = default;

Config::~Config() = default;

Config::Builder Config::init() {
    return Builder{};
}

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
    values_[values_.size() - 2]     = storage_.back()->c_str();
}

const char* const* Config::keywords() const {
    return keywords_.data();
}

const char* const* Config::values() const {
    return values_.data();
}

Config::Builder::Builder() = default;

Config::Builder::Builder(Builder const& other) = default;

Config::Builder::Builder(Builder&& other) = default;

Config::Builder& Config::Builder::operator=(Builder const& other) = default;

Config::Builder& Config::Builder::operator=(Builder&& other) = default;

Config::Builder::~Builder() = default;

Config::Builder& Config::Builder::host(std::string const& val) {
    return set("host", val);
}

Config::Builder& Config::Builder::hostaddr(std::string const& val) {
    return set("hostaddr", val);
}

Config::Builder& Config::Builder::port(int const val) {
    return set("port", val);
}

Config::Builder& Config::Builder::user(std::string const& val) {
    return set("user", val);
}

Config::Builder& Config::Builder::password(std::string const& val) {
    return set("password", val);
}

Config::Builder& Config::Builder::dbname(std::string const& val) {
    return set("dbname", val);
}

Config Config::Builder::build() {
    return std::move(cfg_);
}

}  // namespace postgres
