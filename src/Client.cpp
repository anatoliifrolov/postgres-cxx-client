#include <postgres/Client.h>
#include <postgres/Config.h>
#include <postgres/Command.h>
#include <postgres/Error.h>
#include <postgres/PreparedCommand.h>

namespace postgres {

Client::Client()
    : conn_{Config::build()} {
}

Client::Client(const Config& config)
    : conn_{config} {
}

Client::Client(Client&& other) = default;

Client& Client::operator=(Client&& other) = default;

Client::~Client() = default;

Transaction Client::begin() {
    return Transaction{*this};
}

Result Client::exec(PreparedCommand const& cmd) {
    return conn_.exec(cmd);
}

Result Client::exec(Command const& cmd) {
    return conn_.exec(cmd);
}

Result Client::completeTransaction(Result res) {
    if (!res.isOk()) {
        if (conn_.isOk()) {
            execute("ROLLBACK");
        }
        return res;
    }
    auto commit_res = execute("COMMIT");
    if (!commit_res.isOk()) {
        return commit_res;
    }
    return res;
}

Connection& Client::connection() {
    return conn_;
}

}  // namespace postgres
