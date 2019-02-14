#include <postgres/Client.h>
#include <postgres/Config.h>
#include <postgres/Command.h>
#include <postgres/Error.h>
#include <postgres/PreparedCommand.h>

namespace postgres {

Client::Client() = default;

Client::Client(const Config& config)
    : connection_{config} {
}

Client::Client(Client&& other) = default;

Client& Client::operator=(Client&& other) = default;

Client::~Client() = default;

Result Client::setSchema(const std::string& schema, const bool cache) {
    return validate(trySetSchema(schema, cache));
}

Result Client::trySetSchema(const std::string& schema, const bool cache) {
    auto res = tryExecute("CREATE SCHEMA IF NOT EXISTS " + schema, "SET SCHEMA '" + schema + "'");
    if (cache && res) {
        schema_ = schema;
    }
    return res;
}

Result Client::prepare(const PrepareData& statement, const bool cache) {
    return validate(tryPrepare(statement, cache));
}

Result Client::tryPrepare(const PrepareData& statement, const bool cache) {
    if (cache) {
        prepared_.push_back(statement);
    }
    return connection_.execute(statement);
}

Transaction Client::begin() {
    return Transaction{*this};
}

Result Client::completeTransaction(Result res) {
    if (!res) {
        if (connection_.isOk()) {
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
    _POSTGRES_CXX_ASSERT(tryReconnect(), connection_.error());
}

bool Client::tryReconnect() {
    if (!connection_.reset()) {
        return false;
    }
    if (!schema_.empty() && !trySetSchema(schema_, false)) {
        return false;
    }
    for (auto const& statement : prepared_) {
        tryPrepare(statement, false);
    }
    return true;
}

Connection& Client::connection() {
    return connection_;
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
