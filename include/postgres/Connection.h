#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <libpq-fe.h>
#include <postgres/Fwd.h>

namespace postgres {

class Connection {
public:
    static PGPing ping(Config const& cfg);
    static PGPing ping(std::string const& uri);

    explicit Connection(Config const& cfg);
    explicit Connection(std::string const& uri);
    Connection(Connection const& other) = delete;
    Connection& operator=(Connection const& other) = delete;
    Connection(Connection&& other) noexcept;
    Connection& operator=(Connection&& other) noexcept;
    ~Connection() noexcept;

    Result prepare(PrepareData const& data);
    Result exec(Command const& cmd);
    Result execPrepared(Command const& cmd);
    Status execRaw(std::string_view stmt);

    bool prepareAsync(PrepareData const& data);
    bool execAsync(Command const& cmd);
    bool execPreparedAsync(Command const& cmd);
    bool execRowByRow(Command const& cmd);
    bool execPreparedRowByRow(Command const& cmd);
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
    std::unique_ptr<PGconn, void (*)(PGconn*)> handle_;
};

}  // namespace postgres
