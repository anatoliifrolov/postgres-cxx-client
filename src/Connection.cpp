#include <postgres/Connection.h>

#include <postgres/Command.h>
#include <postgres/Config.h>
#include <postgres/Error.h>
#include <postgres/PrepareData.h>
#include <postgres/Result.h>

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

Connection::Connection(Config const& cfg)
    : handle_{PQconnectdbParams(cfg.keys(), cfg.values(), EXPAND_DBNAME), PQfinish} {
}

Connection::Connection(std::string const& uri)
    : handle_{PQconnectdb(uri.data()), PQfinish} {
}

Connection::Connection(Connection&& other) noexcept = default;

Connection& Connection::operator=(Connection&& other) noexcept = default;

Connection::~Connection() noexcept = default;

Result Connection::prepare(PrepareData const& data) {
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

Result Connection::execPrepared(Command const& cmd) {
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

bool Connection::prepareAsync(PrepareData const& data) {
    return PQsendPrepare(native(),
                         data.name.data(),
                         data.statement.data(),
                         static_cast<int>(data.types.size()),
                         data.types.data()) == 1;
}

bool Connection::execAsync(Command const& cmd) {
    return PQsendQueryParams(native(),
                             cmd.statement(),
                             cmd.count(),
                             cmd.types(),
                             cmd.values(),
                             cmd.lengths(),
                             cmd.formats(),
                             RESULT_FORMAT) == 1;
}

bool Connection::execPreparedAsync(Command const& cmd) {
    return PQsendQueryPrepared(native(),
                               cmd.statement(),
                               cmd.count(),
                               cmd.values(),
                               cmd.lengths(),
                               cmd.formats(),
                               RESULT_FORMAT) == 1;
}

bool Connection::execRowByRow(Command const& cmd) {
    return execAsync(cmd) && (PQsetSingleRowMode(native()) == 1);
}

bool Connection::execPreparedRowByRow(Command const& cmd) {
    return execPreparedAsync(cmd) && (PQsetSingleRowMode(native()) == 1);
}

bool Connection::cancel() {
    auto constexpr BUF_LEN          = 256;
    char           err_buf[BUF_LEN] = {0};
    auto const     info             = PQgetCancel(native());
    auto const     res              = PQcancel(info, err_buf, BUF_LEN) == 1;
    PQfreeCancel(info);
    return res;
}

Result Connection::receive() {
    return Result{PQgetResult(native())};
}

bool Connection::reset() {
    PQreset(native());
    return isOk();
}

bool Connection::isOk() {
    return PQstatus(native()) == CONNECTION_OK;
}

bool Connection::isBusy() {
    PQconsumeInput(native());
    return PQisBusy(native()) == 1;
}

std::string Connection::error() {
    return PQerrorMessage(native());
}

PGconn* Connection::native() const {
    return handle_.get();
}

template <typename C>
std::basic_string<C> doEsc(Connection& conn, C* const escaped) {
    _POSTGRES_CXX_ASSERT(escaped, conn.error());
    std::basic_string<C> res = escaped;
    PQfreemem(escaped);
    return res;
}

std::string Connection::esc(std::string const& in) {
    return doEsc(*this, PQescapeLiteral(native(), in.data(), in.size()));
}

std::string Connection::escId(std::string const& in) {
    return doEsc(*this, PQescapeIdentifier(native(), in.data(), in.size()));
}

std::basic_string<unsigned char> Connection::escBytes(std::basic_string<unsigned char> const& in) {
    size_t len = 0;
    return doEsc(*this, PQescapeByteaConn(native(), in.data(), in.size(), &len));
}

}  // namespace postgres
