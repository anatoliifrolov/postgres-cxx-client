#pragma once

#include <memory>
#include <string>
#include <libpq-fe.h>
#include <postgres/Fwd.h>

namespace postgres {

enum class AsyncMode {
    MULTI_ROW,
    SINGLE_ROW,
};

class Connection {
public:
    static bool ping(Config const& cfg);

    explicit Connection();
    explicit Connection(Config const& cfg);
    Connection(Connection const& other) = delete;
    Connection& operator=(Connection const& other) = delete;
    Connection(Connection&& other) noexcept;
    Connection& operator=(Connection&& other) noexcept;
    ~Connection();

    Result execute(char const* stmt);
    Result execute(std::string const& stmt);
    Result execute(Command const& cmd);
    Result execute(PrepareData const& stmt);
    Result execute(PreparedCommand const& cmd);
    Status executeRaw(char const* stmt);
    Status executeRaw(std::string const& stmt);

    bool send(char const* stmt, AsyncMode mode = AsyncMode::MULTI_ROW);
    bool send(std::string const& stmt, AsyncMode mode = AsyncMode::MULTI_ROW);
    bool send(Command const& cmd, AsyncMode mode = AsyncMode::MULTI_ROW);
    bool send(PrepareData const& stmt);
    bool send(PreparedCommand const& cmd, AsyncMode mode = AsyncMode::MULTI_ROW);
    bool cancel();
    Result receive();

    bool reset();
    bool isOk();
    bool isBusy();
    std::string error();
    PGconn* native() const;

    std::string esc(std::string const& in);
    std::string escId(std::string const& in);
    std::basic_string<unsigned char> esc(std::basic_string<unsigned char> const& in);

private:
    std::unique_ptr<PGconn, void (*)(PGconn*)> handle_;
};

}  // namespace postgres
