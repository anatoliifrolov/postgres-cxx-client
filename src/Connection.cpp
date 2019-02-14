#include <postgres/Connection.h>

#include <postgres/Command.h>
#include <postgres/Config.h>
#include <postgres/Error.h>
#include <postgres/PreparedCommand.h>
#include <postgres/PrepareData.h>
#include <postgres/Result.h>

namespace postgres {

enum {
    EXPAND_DB_NAME = 0,
    RESULT_FORMAT  = 1,
};

bool Connection::ping(Config const& cfg) {
    switch (PQpingParams(cfg.keys(), cfg.values(), EXPAND_DB_NAME)) {
        case PGPing::PQPING_OK:
        case PGPing::PQPING_REJECT: {
            return true;
        }
        case PGPing::PQPING_NO_RESPONSE:
        case PGPing::PQPING_NO_ATTEMPT: {
            return false;
        }
    }
    return false;
}

Connection::Connection()
    : Connection{Config::make()} {
}

Connection::Connection(Config const& cfg)
    : handle_{PQconnectdbParams(cfg.keys(), cfg.values(), EXPAND_DB_NAME), PQfinish} {
}

Connection::Connection(Connection&& other) noexcept = default;

Connection& Connection::operator=(Connection&& other) noexcept = default;

Connection::~Connection() = default;

Result Connection::execute(char const* const stmt) {
    return execute(Command{stmt});
}

Result Connection::execute(std::string const& stmt) {
    return execute(Command{stmt});
}

Result Connection::execute(Command const& cmd) {
    return Result{PQexecParams(native(),
                               cmd.statement(),
                               cmd.count(),
                               cmd.types(),
                               cmd.values(),
                               cmd.lengths(),
                               cmd.formats(),
                               RESULT_FORMAT)};
}

Result Connection::execute(PrepareData const& stmt) {
    return Result{PQprepare(native(),
                            stmt.name.c_str(),
                            stmt.body.c_str(),
                            static_cast<int>(stmt.types.size()),
                            stmt.types.empty() ? nullptr : stmt.types.data())};
}

Result Connection::execute(PreparedCommand const& cmd) {
    return Result{PQexecPrepared(native(),
                                 cmd.statement(),
                                 cmd.count(),
                                 cmd.values(),
                                 cmd.lengths(),
                                 cmd.formats(),
                                 RESULT_FORMAT)};
}

Status Connection::executeRaw(char const* const stmt) {
    return Status{PQexec(native(), stmt)};
}

Status Connection::executeRaw(std::string const& stmt) {
    return executeRaw(stmt.c_str());
}

bool Connection::send(char const* const stmt, AsyncMode const mode) {
    return send(Command{stmt}, mode);
}

bool Connection::send(std::string const& stmt, AsyncMode const mode) {
    return send(Command{stmt}, mode);
}

bool Connection::send(Command const& cmd, AsyncMode const mode) {
    auto const res = PQsendQueryParams(native(),
                                       cmd.statement(),
                                       cmd.count(),
                                       cmd.types(),
                                       cmd.values(),
                                       cmd.lengths(),
                                       cmd.formats(),
                                       RESULT_FORMAT) == 1;
    if (res && (mode == AsyncMode::SINGLE_ROW)) {
        PQsetSingleRowMode(native());
    }
    return res;
}

bool Connection::send(PrepareData const& stmt) {
    return PQsendPrepare(native(),
                         stmt.name.c_str(),
                         stmt.body.c_str(),
                         static_cast<int>(stmt.types.size()),
                         stmt.types.empty() ? nullptr : stmt.types.data()) == 1;
}

bool Connection::send(PreparedCommand const& cmd, AsyncMode const mode) {
    auto const res = PQsendQueryPrepared(native(),
                                         cmd.statement(),
                                         cmd.count(),
                                         cmd.values(),
                                         cmd.lengths(),
                                         cmd.formats(),
                                         RESULT_FORMAT) == 1;
    if (res && (mode == AsyncMode::SINGLE_ROW)) {
        PQsetSingleRowMode(native());
    }
    return res;
}

bool Connection::cancel() {
    static auto constexpr kErrBufSize          = 256;
    static char           err_buf[kErrBufSize] = {0};
    auto const            info                 = PQgetCancel(native());
    auto const            res                  = PQcancel(info, err_buf, kErrBufSize) == 1;
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
    return PQstatus(native()) == ConnStatusType::CONNECTION_OK;
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
    return doEsc(*this, PQescapeLiteral(native(), in.c_str(), in.size()));
}

std::string Connection::escId(std::string const& in) {
    return doEsc(*this, PQescapeIdentifier(native(), in.c_str(), in.size()));
}

std::basic_string<unsigned char> Connection::esc(std::basic_string<unsigned char> const& in) {
    auto len = size_t{};
    return doEsc(*this, PQescapeByteaConn(native(), in.c_str(), in.size(), &len));
}

}  // namespace postgres
