#include <postgres/Client.h>
#include <postgres/Config.h>
#include <postgres/Command.h>
#include <postgres/Error.h>
#include <postgres/PreparedCommand.h>

namespace postgres {

Client::Client()
    : conn_{Config::make()} {
}

Client::Client(const Config& config)
    : conn_{config} {
}

Client::Client(Client&& other) = default;

Client& Client::operator=(Client&& other) = default;

Client::~Client() = default;

Result Client::prepare(const PrepareData& statement) {
    return validate(tryPrepare(statement));
}

Result Client::tryPrepare(const PrepareData& statement) {
    return conn_.prepare(statement);
}

Transaction Client::begin() {
    return Transaction{*this};
}

Result Client::doTryExecute(std::string&& stmt) {
    return conn_.exec(Command{std::move(stmt)});
}

Result Client::doTryExecute(std::string const& stmt) {
    return conn_.exec(Command{stmt});
}

Result Client::doTryExecute(std::string_view stmt) {
    return conn_.exec(Command{stmt});
}

Result Client::doTryExecute(char const* stmt) {
    return conn_.exec(Command{stmt});
}

Result Client::doTryExecute(PreparedCommand const& cmd) {
    return conn_.execPrepared(cmd);
}

Result Client::doTryExecute(Command const& cmd) {
    return conn_.exec(cmd);
}

Result Client::completeTransaction(Result res) {
    if (!res) {
        if (conn_.isOk()) {
            tryExecute("ROLLBACK");
        }
        return res;
    }
    auto commit_res = tryExecute("COMMIT");
    if (!commit_res) {
        return commit_res;
    }
    return res;
}

void Client::reconnect() {
    _POSTGRES_CXX_ASSERT(tryReconnect(), conn_.error());
}

bool Client::tryReconnect() {
    return conn_.reset();
}

Connection& Client::connection() {
    return conn_;
}

Result Client::validate(postgres::Result res) {
    if (res.isOk()) {
        return res;
    }
    std::string msg = res.errorMessage();
    if (msg.empty()) {
        msg = connection().error();
    }
    _POSTGRES_CXX_FAIL(msg);
}

}  // namespace postgres
