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

Result Client::exec(PreparedCommand const& cmd) {
    return conn_.exec(cmd);
}

Result Client::exec(Command const& cmd) {
    return conn_.exec(cmd);
}

Connection& Client::connection() {
    return conn_;
}

}  // namespace postgres
