#include <postgres/Connection.h>

#include <postgres/Config.h>
#include <postgres/Consumer.h>
#include <postgres/Error.h>
#include <postgres/PreparedCommand.h>
#include <postgres/PrepareData.h>
#include <postgres/Receiver.h>

namespace postgres {

enum {
    EXPAND_DBNAME = 0,
    RESULT_FORMAT = 1,
};

PGPing Connection::ping() {
    return ping(Config::build());
}

PGPing Connection::ping(Config const& cfg) {
    return PQpingParams(cfg.keys(), cfg.values(), EXPAND_DBNAME);
}

PGPing Connection::ping(std::string const& uri) {
    return PQping(uri.data());
}

Connection::Connection()
    : Connection{Config::build()} {
}

Connection::Connection(Config const& cfg)
    : handle_{PQconnectdbParams(cfg.keys(), cfg.values(), EXPAND_DBNAME), PQfinish} {
}

Connection::Connection(std::string const& uri)
    : handle_{PQconnectdb(uri.data()), PQfinish} {
}

Connection::Connection(Connection&& other) noexcept = default;

Connection& Connection::operator=(Connection&& other) noexcept = default;

Connection::~Connection() noexcept = default;

Result Connection::exec(PrepareData const& prep) {
    return Result{PQprepare(native(),
                            prep.name.data(),
                            prep.statement.data(),
                            static_cast<int>(prep.types.size()),
                            prep.types.data())};
}

Result Connection::exec(Command const& cmd) {
    return Result{PQexecParams(native(),
                               cmd.statement(),
                               cmd.count(),
                               cmd.types(),
                               cmd.values(),
                               cmd.lengths(),
                               cmd.formats(),
                               RESULT_FORMAT)};
}

Result Connection::exec(PreparedCommand const& cmd) {
    return Result{PQexecPrepared(native(),
                                 cmd.statement(),
                                 cmd.count(),
                                 cmd.values(),
                                 cmd.lengths(),
                                 cmd.formats(),
                                 RESULT_FORMAT)};
}

Status Connection::execRaw(std::string_view const stmt) {
    return Status{PQexec(native(), stmt.data())};
}

Receiver Connection::send(PrepareData const& prep) {
    return Receiver{handle_,
                    PQsendPrepare(native(),
                                  prep.name.data(),
                                  prep.statement.data(),
                                  static_cast<int>(prep.types.size()),
                                  prep.types.data())};
}

Receiver Connection::send(Command const& cmd) {
    return Receiver{handle_,
                    PQsendQueryParams(native(),
                                      cmd.statement(),
                                      cmd.count(),
                                      cmd.types(),
                                      cmd.values(),
                                      cmd.lengths(),
                                      cmd.formats(),
                                      RESULT_FORMAT)};
}

Receiver Connection::send(PreparedCommand const& cmd) {
    return Receiver{handle_,
                    PQsendQueryPrepared(native(),
                                        cmd.statement(),
                                        cmd.count(),
                                        cmd.values(),
                                        cmd.lengths(),
                                        cmd.formats(),
                                        RESULT_FORMAT)};
}

Consumer Connection::sendRaw(std::string_view const stmt) {
    return Consumer{handle_, PQsendQuery(native(), stmt.data())};
}

Receiver Connection::iter(Command const& cmd) {
    auto rcvr = send(cmd);
    rcvr.iter();
    return rcvr;
}

Receiver Connection::iter(PreparedCommand const& cmd) {
    auto rcvr = send(cmd);
    rcvr.iter();
    return rcvr;
}

Transaction Connection::begin() {
    return Transaction{*this, exec("BEGIN")};
}

void Connection::check() {
    _POSTGRES_CXX_ASSERT(RuntimeError, isOk(), "connection is in a bad state: " << message());
}

bool Connection::reset() {
    PQreset(native());
    return isOk();
}

bool Connection::isOk() {
    return PQstatus(native()) == CONNECTION_OK;
}

std::string Connection::message() {
    return PQerrorMessage(native());
}

std::string Connection::esc(std::string const& in) {
    return doEsc(in, PQescapeLiteral);
}

std::string Connection::escId(std::string const& in) {
    return doEsc(in, PQescapeIdentifier);
}

template <typename F>
std::string Connection::doEsc(std::string const& in, F const f) {
    auto const escaped = f(native(), in.data(), in.size());
    _POSTGRES_CXX_ASSERT(RuntimeError, (escaped != nullptr), "fail to escape: " << message());
    std::string res = escaped;
    PQfreemem(escaped);
    return res;
}

PGconn* Connection::native() const {
    return handle_.get();
}

}  // namespace postgres
