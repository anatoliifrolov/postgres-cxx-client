#pragma once

#include <memory>
#include <string>
#include <libpq-fe.h>
#include <postgres/Result.h>

namespace postgres {

class Config;
class Command;
class PreparedCommand;
struct PrepareData;

bool ping(const Config& config);

class Connection {
public:
    explicit Connection(const Config& config);
    Connection(const Connection& other) = delete;
    Connection(Connection&& other);
    Connection& operator=(const Connection& other) = delete;
    Connection& operator=(Connection&& other);
    ~Connection();

    // Status info.
    bool isOk();
    bool isBusy();
    std::string errorMessage();

    // Escaping.
    std::string esc(const std::string& in);
    std::string escId(const std::string& in);
    std::basic_string<unsigned char> escBytes(const std::basic_string<unsigned char>& in);

    // Statement execution.
    Status executeRaw(const std::string& statement);
    Status executeRaw(const char* const statement);
    Result execute(const PrepareData& statement);
    Result execute(const std::string& statement);
    Result execute(const char* const statement);
    Result execute(const PreparedCommand& command);
    Result execute(const Command& command);

    // Async interface.
    bool send(const PrepareData& statement);
    bool send(const std::string& statement, const bool row_by_row = false);
    bool send(const char* const statement, const bool row_by_row = false);
    bool send(const PreparedCommand& command, const bool row_by_row = false);
    bool send(const Command& command, const bool row_by_row = false);
    bool cancel();
    Result nextResult();

    // Connection management.
    bool reset();
    PGconn* native() const;

private:
    template <typename Char>
    std::basic_string<Char> doEsc(Char* const res);

    std::unique_ptr<PGconn, void(*)(PGconn*)> handle_;
};

}  // namespace postgres