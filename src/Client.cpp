#include <postgres/Client.h>

#include <postgres/Config.h>
#include <postgres/Consumer.h>
#include <postgres/Error.h>
#include <postgres/PreparedCommand.h>
#include <postgres/PreparingStatement.h>
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

Result Client::exec(PreparingStatement const& stmt) {
    return Result{PQprepare(native(),
                            stmt.name.data(),
                            stmt.body.data(),
                            static_cast<int>(stmt.types.size()),
                            stmt.types.data())};
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

Status Client::execRaw(std::string_view stmt) {
    return Status{PQexec(native(), stmt.data())};
}

Receiver Client::send(PreparingStatement const& stmt) {
    return Receiver{conn_,
                    PQsendPrepare(native(),
                                  stmt.name.data(),
                                  stmt.body.data(),
                                  static_cast<int>(stmt.types.size()),
                                  stmt.types.data())};
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

Consumer Client::sendRaw(std::string_view stmt) {
    return Consumer{conn_, PQsendQuery(native(), stmt.data())};
}

Receiver Client::iter(Command const& cmd) {
    auto rcvr = send(cmd);
    rcvr.iter();
    return rcvr;
}

Receiver Client::iter(PreparedCommand const& cmd) {
    auto rcvr = send(cmd);
    rcvr.iter();
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
