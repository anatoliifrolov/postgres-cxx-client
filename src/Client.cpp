#include <postgres/Client.h>

#include <utility>
#include <postgres/internal/Channel.h>
#include <postgres/internal/Dispatcher.h>
#include <postgres/Context.h>
#include <postgres/Result.h>
#include <postgres/Status.h>

namespace postgres {

Client::Client()
    : Client{Context{}} {
}

Client::Client(Context ctx) {
    auto pctx = std::make_shared<Context>(std::move(ctx));
    auto chan = std::make_shared<internal::Channel>(pctx);
    impl_ = std::make_unique<Impl>(std::move(pctx), std::move(chan));
}

Client::Client(Client&& other) noexcept = default;

Client& Client::operator=(Client&& other) noexcept = default;

Client::~Client() noexcept = default;

std::future<Status> Client::exec(std::function<Status(Connection&)> job) {
    return impl_->send(std::move(job));
}

std::future<Result> Client::query(std::function<Result(Connection&)> job) {
    return impl_->send(std::move(job));
}

}  // namespace postgres
