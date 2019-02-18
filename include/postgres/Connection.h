#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <libpq-fe.h>

namespace postgres {

class Command;
class Config;
class Receiver;
class Result;
struct PrepareData;

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
    Result execRaw(std::string_view stmt);

    Receiver prepareAsync(PrepareData const& data);
    Receiver send(Command const& cmd);
    Receiver sendPrepared(Command const& cmd);
    Receiver sendRaw(std::string_view stmt);

    bool reset();
    bool isOk();
    std::string message();

    std::string esc(std::string const& in);
    std::string escId(std::string const& in);

    PGconn* native() const;

private:
    std::shared_ptr<PGconn> handle_;
};

}  // namespace postgres
