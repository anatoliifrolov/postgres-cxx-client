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
    Connection(Connection const& other);
    Connection& operator=(Connection const& other);
    Connection(Connection&& other) noexcept;
    Connection& operator=(Connection&& other) noexcept;
    ~Connection() noexcept;

    Result prepare(PrepareData const& data);
    Result exec(Command const& cmd);
    Result execPrepared(Command const& cmd);
    Result execRaw(std::string_view stmt);

    Receiver prepareAsync(PrepareData const& data);
    Receiver execAsync(Command const& cmd);
    Receiver execPreparedAsync(Command const& cmd);
    Receiver execRawAsync(std::string_view stmt);

    bool reset();
    bool isOk();
    std::string error();
    PGconn* native() const;

    std::string esc(std::string const& in);
    std::string escId(std::string const& in);
    std::basic_string<unsigned char> escBytes(std::basic_string<unsigned char> const& in);

private:
    std::shared_ptr<PGconn> handle_;
};

}  // namespace postgres
