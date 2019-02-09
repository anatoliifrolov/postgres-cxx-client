#include <postgres/Connection.h>
#include <postgres/Config.h>
#include <postgres/PrepareData.h>
#include <postgres/Command.h>
#include <postgres/PreparedCommand.h>
#include <postgres/internal/Assert.h>

namespace postgres {

Connection::Connection() : Connection{Config{}} {}

Connection::Connection(const Config& config)
    : handle_{
        PQconnectdbParams(
            config.keywords(),
            config.values(),
            0),
        PQfinish}
{}

Connection::Connection(Connection&& other) = default;

Connection& Connection::operator=(Connection&& other) = default;

Connection::~Connection() = default;

bool Connection::ping(const Config& config) {
    auto const status = PQpingParams(
        config.keywords(),
        config.values(),
        0);
    switch (status) {
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

bool Connection::isOk() {
    return PQstatus(native()) == ConnStatusType::CONNECTION_OK;
}

bool Connection::isBusy() {
    PQconsumeInput(native());
    return PQisBusy(native()) == 1;
}

std::string Connection::errorMessage() {
    return PQerrorMessage(native());
}

std::string Connection::esc(const std::string& in) {
    return doEsc(PQescapeLiteral(native(), in.c_str(), in.size()));
}

std::string Connection::escId(const std::string& in) {
    return doEsc(PQescapeIdentifier(native(), in.c_str(), in.size()));
}

std::basic_string<unsigned char> Connection::escBytes(const std::basic_string<unsigned char>& in) {
    auto len = size_t{};
    return doEsc(PQescapeByteaConn(native(), in.c_str(), in.size(), &len));
}

template <typename Char>
std::basic_string<Char> Connection::doEsc(Char* const escaped) {
    _POSTGRES_CXX_ASSERT(escaped, errorMessage());
    std::basic_string<Char> res = escaped;
    PQfreemem(escaped);
    return res;
}

Status Connection::executeRaw(const std::string& statement) {
    return executeRaw(statement.c_str());
}

Status Connection::executeRaw(const char* const statement) {
    return Status{PQexec(native(), statement)};
}

Result Connection::execute(const PrepareData& statement) {
    return Result{PQprepare(
        native(),
        statement.name_.c_str(),
        statement.body_.c_str(),
        statement.types_.size(),
        statement.types_.empty() ? nullptr : statement.types_.data())};
}

Result Connection::execute(const std::string& statement) {
    return execute(Command{statement});
}

Result Connection::execute(const char* const statement) {
    return execute(Command{statement});
}

Result Connection::execute(const PreparedCommand& command) {
    return Result{PQexecPrepared(
        native(),
        command.statement(),
        command.nParams(),
        command.paramValues(),
        command.paramLengths(),
        command.paramFormats(),
        command.resultFormat())};
}

Result Connection::execute(const Command& command) {
    return Result{PQexecParams(
        native(),
        command.statement(),
        command.nParams(),
        command.paramTypes(),
        command.paramValues(),
        command.paramLengths(),
        command.paramFormats(),
        command.resultFormat())};
}

bool Connection::send(const PrepareData& statement) {
    return PQsendPrepare(
        native(),
        statement.name_.c_str(),
        statement.body_.c_str(),
        statement.types_.size(),
        statement.types_.empty() ? nullptr : statement.types_.data()) == 1;
}

bool Connection::send(const std::string& statement, const bool row_by_row) {
    return send(Command{statement}, row_by_row);
}

bool Connection::send(const char* const statement, const bool row_by_row) {
    return send(Command{statement}, row_by_row);
}

bool Connection::send(const PreparedCommand& command, const bool row_by_row) {
    auto const res = PQsendQueryPrepared(
        native(),
        command.statement(),
        command.nParams(),
        command.paramValues(),
        command.paramLengths(),
        command.paramFormats(),
        command.resultFormat()) == 1;
    if (res && row_by_row) {
        PQsetSingleRowMode(native());
    }
    return res;
}

bool Connection::send(const Command& command, const bool row_by_row) {
    auto const res = PQsendQueryParams(
        native(),
        command.statement(),
        command.nParams(),
        command.paramTypes(),
        command.paramValues(),
        command.paramLengths(),
        command.paramFormats(),
        command.resultFormat()) == 1;
    if (res && row_by_row) {
        PQsetSingleRowMode(native());
    }
    return res;
}

bool Connection::cancel() {
    static auto constexpr kErrBufSize = 256;
    static char err_buf[kErrBufSize] = {0};
    auto const info = PQgetCancel(native());
    auto const res = PQcancel(info, err_buf, kErrBufSize) == 1;
    PQfreeCancel(info);
    return res;
}

Result Connection::nextResult() {
    return Result{PQgetResult(native())};
}

bool Connection::reset() {
    PQreset(native());
    return isOk();
}

PGconn* Connection::native() const {
    return handle_.get();
}

}  // namespace postgres
