#include <postgres/Config.h>

namespace postgres {

Config::Config() = default;

Config::Config(Config&& other) noexcept = default;

Config& Config::operator=(Config&& other) noexcept = default;

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

Config::Builder::Builder(Builder&& other) noexcept = default;

Config::Builder& Config::Builder::operator=(Builder&& other) noexcept = default;

Config::Builder::~Builder() = default;

Config::Builder& Config::Builder::application_name(std::string const& val) {
    return set("application_name", val);
}

Config::Builder& Config::Builder::client_encoding(std::string const& val) {
    return set("client_encoding", val);
}

Config::Builder& Config::Builder::connect_timeout(Duration const val) {
    return setInterval("connect_timeout", val);
}

Config::Builder& Config::Builder::dbname(std::string const& val) {
    return set("dbname", val);
}

Config::Builder& Config::Builder::fallback_application_name(std::string const& val) {
    return set("fallback_application_name", val);
}

Config::Builder& Config::Builder::gsslib(std::string const& val) {
    return set("gsslib", val);
}

Config::Builder& Config::Builder::host(std::string const& val) {
    return set("host", val);
}

Config::Builder& Config::Builder::hostaddr(std::string const& val) {
    return set("hostaddr", val);
}

Config::Builder& Config::Builder::keepalives(bool const val) {
    return val ? enable("keepalives") : disable("keepalives");
}

Config::Builder& Config::Builder::keepalives_count(int const val) {
    return setNumber("keepalives_count", val);
}

Config::Builder& Config::Builder::keepalives_idle(Duration const val) {
    return setInterval("keepalives_idle", val);
}

Config::Builder& Config::Builder::keepalives_interval(Duration const val) {
    return setInterval("keepalives_interval", val);
}

Config::Builder& Config::Builder::krbsrvname(std::string const& val) {
    return set("krbsrvname", val);
}

Config::Builder& Config::Builder::options(std::string const& val) {
    return set("options", val);
}

Config::Builder& Config::Builder::passfile(std::string const& val) {
    return set("passfile", val);
}

Config::Builder& Config::Builder::password(std::string const& val) {
    return set("password", val);
}

Config::Builder& Config::Builder::port(int const val) {
    return setNumber("port", val);
}

Config::Builder& Config::Builder::requirepeer(std::string const& val) {
    return set("requirepeer", val);
}

Config::Builder& Config::Builder::requiressl(bool const val) {
    return val ? enable("requiressl") : disable("requiressl");
}

Config::Builder& Config::Builder::service(std::string const& val) {
    return set("service", val);
}

Config::Builder& Config::Builder::sslcert(std::string const& val) {
    return set("sslcert", val);
}

Config::Builder& Config::Builder::sslcompression(bool const val) {
    return val ? enable("sslcompression") : disable("sslcompression");
}

Config::Builder& Config::Builder::sslcrl(std::string const& val) {
    return set("sslcrl", val);
}

Config::Builder& Config::Builder::sslkey(std::string const& val) {
    return set("sslkey", val);
}

Config::Builder& Config::Builder::sslmode(SslMode const val) {
    return set("sslmode", [val] {
        switch (val) {
            case SslMode::ALLOW: {
                return "allow";
            }
            case SslMode::DISABLE: {
                return "disable";
            }
            case SslMode::PREFER: {
                return "prefer";
            }
            case SslMode::REQUIRE: {
                return "require";
            }
            case SslMode::VERIFY_CA: {
                return "verify-ca";
            }
            case SslMode::VERIFY_FULL: {
                return "verify-full";
            }
            case SslMode::DEFAULT: {
                break;
            }
        }
        return "prefer";
    }());
}

Config::Builder& Config::Builder::sslrootcert(std::string const& val) {
    return set("sslrootcert", val);
}

Config::Builder& Config::Builder::target_session_attrs(TargetSession val) {
    return set("target_session_attrs", [val] {
        switch (val) {
            case TargetSession::ANY: {
                return "any";
            }
            case TargetSession::READ_WRITE: {
                return "read-write";
            }
            case TargetSession::DEFAULT: {
                break;
            }
        }
        return "any";
    }());
}

Config::Builder& Config::Builder::user(std::string const& val) {
    return set("user", val);
}

Config::Builder& Config::Builder::disable(std::string const& key) {
    return set(key, "0");
}

Config::Builder& Config::Builder::enable(std::string const& key) {
    return set(key, "1");
}

Config::Builder& Config::Builder::setNumber(std::string const& key, int val) {
    return set(key, std::to_string(val));
}

Config::Builder& Config::Builder::setInterval(std::string const& key, Duration val) {
    return set(key, std::to_string(std::chrono::duration_cast<std::chrono::seconds>(val).count()));
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
