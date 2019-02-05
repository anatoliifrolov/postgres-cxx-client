#pragma once

#include <string>
#include <list>
#include <vector>
#include <memory>

namespace postgres {

class Config {
public:
    explicit Config(
        const std::string& dbname = "",
        const std::string& user = "",
        const std::string& password = "",
        const std::string& host = "127.0.0.1",
        const int port = 5432);
    Config(const Config&);
    Config(Config&&);
    Config& operator=(const Config&);
    Config& operator=(Config&&);
    ~Config();

    // Any parameter supported by pq library.
    const char* get(const char* const key) const;
    void set(const char* const key, const std::string& val);
    void set(const char* const key, const char* const val);

    template <typename T>
    void set(const char* const key, const T& val) {
        set(key, std::to_string(val));
    }

    // libpq interface adapters.
    const char* const* keywords() const;
    const char* const* values() const;
    int expandDbname() const;

private:
    std::list<std::shared_ptr<std::string>> storage_;
    std::vector<const char*> keywords_;
    std::vector<const char*> values_;
};

}  // namespace postgres
