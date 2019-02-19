#include <postgres/Connection.h>

#include <postgres/Config.h>
#include <postgres/Error.h>
#include <postgres/PreparedCommand.h>
#include <postgres/PrepareData.h>
#include <postgres/Receiver.h>

namespace postgres {

enum {
    EXPAND_DBNAME = 0,
    RESULT_FORMAT = 1,
};

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
    : conn_{PQconnectdbParams(cfg.keys(), cfg.values(), EXPAND_DBNAME), PQfinish} {
}

Connection::Connection(std::string const& uri)
    : conn_{PQconnectdb(uri.data()), PQfinish} {
}

Connection::Connection(Connection&& other) noexcept = default;

Connection& Connection::operator=(Connection&& other) noexcept = default;

Connection::~Connection() noexcept = default;

Result Connection::exec(PrepareData const& data) {
    return Result{PQprepare(native(),
                            data.name.data(),
                            data.statement.data(),
                            static_cast<int>(data.types.size()),
                            data.types.data())};
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

Result Connection::execRaw(std::string_view stmt) {
    return Result{PQexec(native(), stmt.data())};
}

Receiver Connection::send(PrepareData const& data) {
    return Receiver{conn_,
                    PQsendPrepare(native(),
                                  data.name.data(),
                                  data.statement.data(),
                                  static_cast<int>(data.types.size()),
                                  data.types.data())};
}

Receiver Connection::send(Command const& cmd) {
    return Receiver{conn_,
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
    return Receiver{conn_,
                    PQsendQueryPrepared(native(),
                                        cmd.statement(),
                                        cmd.count(),
                                        cmd.values(),
                                        cmd.lengths(),
                                        cmd.formats(),
                                        RESULT_FORMAT)};
}

Receiver Connection::sendRaw(std::string_view stmt) {
    return Receiver{conn_, PQsendQuery(native(), stmt.data())};
}

Receiver Connection::iter(Command const& cmd) {
    auto rcvr = send(cmd);
    rcvr.setRowByRow();
    return rcvr;
}

Receiver Connection::iter(PreparedCommand const& cmd) {
    auto rcvr = send(cmd);
    rcvr.setRowByRow();
    return rcvr;
}

Transaction Connection::begin() {
    return Transaction{*this};
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
    auto const escaped = PQescapeLiteral(native(), in.data(), in.size());
    _POSTGRES_CXX_ASSERT(escaped != nullptr, message());
    std::string res = escaped;
    PQfreemem(escaped);
    return res;
}

std::string Connection::escId(std::string const& in) {
    auto const escaped = PQescapeIdentifier(native(), in.data(), in.size());
    _POSTGRES_CXX_ASSERT(escaped != nullptr, message());
    std::string res = escaped;
    PQfreemem(escaped);
    return res;
}

PGconn* Connection::native() const {
    return conn_.get();
}

}  // namespace postgres
