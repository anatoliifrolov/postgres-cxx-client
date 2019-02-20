#include <postgres/Client.h>

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

PGPing Client::ping() {
    return ping(Config::build());
}

PGPing Client::ping(Config const& cfg) {
    return PQpingParams(cfg.keys(), cfg.values(), EXPAND_DBNAME);
}

PGPing Client::ping(std::string const& uri) {
    return PQping(uri.data());
}

Client::Client()
    : Client{Config::build()} {
}

Client::Client(Config const& cfg)
    : conn_{PQconnectdbParams(cfg.keys(), cfg.values(), EXPAND_DBNAME), PQfinish} {
}

Client::Client(std::string const& uri)
    : conn_{PQconnectdb(uri.data()), PQfinish} {
}

Client::Client(Client&& other) noexcept = default;

Client& Client::operator=(Client&& other) noexcept = default;

Client::~Client() noexcept = default;

Result Client::exec(PrepareData const& data) {
    return Result{PQprepare(native(),
                            data.name.data(),
                            data.statement.data(),
                            static_cast<int>(data.types.size()),
                            data.types.data())};
}

Result Client::exec(Command const& cmd) {
    return Result{PQexecParams(native(),
                               cmd.statement(),
                               cmd.count(),
                               cmd.types(),
                               cmd.values(),
                               cmd.lengths(),
                               cmd.formats(),
                               RESULT_FORMAT)};
}

Result Client::exec(PreparedCommand const& cmd) {
    return Result{PQexecPrepared(native(),
                                 cmd.statement(),
                                 cmd.count(),
                                 cmd.values(),
                                 cmd.lengths(),
                                 cmd.formats(),
                                 RESULT_FORMAT)};
}

Result Client::execRaw(std::string_view stmt) {
    return Result{PQexec(native(), stmt.data())};
}

Receiver Client::send(PrepareData const& data) {
    return Receiver{conn_,
                    PQsendPrepare(native(),
                                  data.name.data(),
                                  data.statement.data(),
                                  static_cast<int>(data.types.size()),
                                  data.types.data())};
}

Receiver Client::send(Command const& cmd) {
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

Receiver Client::send(PreparedCommand const& cmd) {
    return Receiver{conn_,
                    PQsendQueryPrepared(native(),
                                        cmd.statement(),
                                        cmd.count(),
                                        cmd.values(),
                                        cmd.lengths(),
                                        cmd.formats(),
                                        RESULT_FORMAT)};
}

Receiver Client::sendRaw(std::string_view stmt) {
    return Receiver{conn_, PQsendQuery(native(), stmt.data())};
}

Receiver Client::iter(Command const& cmd) {
    auto rcvr = send(cmd);
    rcvr.setRowByRow();
    return rcvr;
}

Receiver Client::iter(PreparedCommand const& cmd) {
    auto rcvr = send(cmd);
    rcvr.setRowByRow();
    return rcvr;
}

Transaction Client::begin() {
    return Transaction{*this};
}

bool Client::reset() {
    PQreset(native());
    return isOk();
}

bool Client::isOk() {
    return PQstatus(native()) == CONNECTION_OK;
}

std::string Client::message() {
    return PQerrorMessage(native());
}

std::string Client::esc(std::string const& in) {
    return postEsc(PQescapeLiteral(native(), in.data(), in.size()));
}

std::string Client::escId(std::string const& in) {
    return postEsc(PQescapeIdentifier(native(), in.data(), in.size()));
}

std::string Client::postEsc(char* const escaped) {
    _POSTGRES_CXX_ASSERT(escaped != nullptr, message());
    std::string res = escaped;
    PQfreemem(escaped);
    return res;
}

PGconn* Client::native() const {
    return conn_.get();
}

}  // namespace postgres
