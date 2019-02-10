#include <postgres/Config.h>

namespace postgres {

Config::Config() = default;

Config::Config(Config&& other) = default;

Config& Config::operator=(Config&& other) = default;

Config::~Config() = default;

Config Config::make() {
    return Builder{}.build();
}

char const* const* Config::keys() const {
    return keys_.data();
}

char const* const* Config::values() const {
    return vals_.data();
}

Config::Builder::Builder() = default;

Config::Builder::Builder(Builder&& other) = default;

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

Config::Builder& Config::Builder::set(std::string const& key, int const val) {
    return set(key, std::to_string(val));
}

Config::Builder& Config::Builder::set(std::string const& key, std::string const& val) {
    cfg_.params_[key] = val;
    return *this;
}

Config Config::Builder::build() {
    for (auto const& param : cfg_.params_) {
        cfg_.keys_.push_back(param.first.c_str());
        cfg_.vals_.push_back(param.second.c_str());
    }
    cfg_.keys_.push_back(nullptr);
    cfg_.vals_.push_back(nullptr);
    return std::move(cfg_);
}

}  // namespace postgres
