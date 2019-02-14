#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <libpq-fe.h>
#include <postgres/Fwd.h>

namespace postgres {

enum class AsyncMode {
    MULTI_ROW,
    SINGLE_ROW,
};

class Connection {
public:
    static PGPing ping(Config const& cfg);
    static PGPing ping(std::string const& uri);

    explicit Connection();
    explicit Connection(Config const& cfg);
    explicit Connection(std::string const& uri);
    Connection(Connection const& other) = delete;
    Connection& operator=(Connection const& other) = delete;
    Connection(Connection&& other) noexcept;
    Connection& operator=(Connection&& other) noexcept;
    ~Connection();

    Result execute(std::string&& stmt);
    Result execute(std::string const& stmt);
    Result execute(std::string_view stmt);
    Result execute(char const* stmt);
    Result execute(Command const& cmd);
    Result execute(PrepareData const& stmt);
    Result execute(PreparedCommand const& cmd);
    Status executeRaw(std::string const& stmt);
    Status executeRaw(std::string_view stmt);
    Status executeRaw(char const* stmt);

    bool send(std::string&& stmt, AsyncMode mode = AsyncMode::MULTI_ROW);
    bool send(std::string const& stmt, AsyncMode mode = AsyncMode::MULTI_ROW);
    bool send(std::string_view stmt, AsyncMode mode = AsyncMode::MULTI_ROW);
    bool send(char const* stmt, AsyncMode mode = AsyncMode::MULTI_ROW);
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
    std::basic_string<unsigned char> escBytes(std::basic_string<unsigned char> const& in);

private:
    bool onSend(int res, AsyncMode mode);

    std::unique_ptr<PGconn, void (*)(PGconn*)> handle_;
};

}  // namespace postgres
