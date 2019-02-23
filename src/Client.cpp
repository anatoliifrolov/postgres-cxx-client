#include <postgres/Client.h>

#include <thread>
#include <utility>

namespace postgres {

Client::Client(internal::Context ctx)
    : ctx_{std::move(ctx)} {
}

Client::Client(Client&& other) noexcept = default;

Client& Client::operator=(Client&& other) noexcept = default;

Client::~Client() noexcept = default;

Client Client::build() {
    return Builder{}.build();
}

Client::Builder::Builder()
    : ctx_{Config::build()} {
    ctx_.max_pool_size = std::thread::hardware_concurrency();
    ctx_.max_queue_size = 0;
}

Client::Builder::Builder(Client::Builder&& other) noexcept = default;

Client::Builder& Client::Builder::operator=(Client::Builder&& other) noexcept = default;

Client::Builder::~Builder() noexcept = default;

Client::Builder& Client::Builder::config(Config cfg) {
    ctx_.cfg = std::move(cfg);
    return *this;
}

Client::Builder& Client::Builder::uri(std::string uri) {
    ctx_.uri = std::move(uri);
    return *this;
}

Client::Builder& Client::Builder::prepare(PreparingStatement stmt) {
    ctx_.prep.push_back(std::move(stmt));
    return *this;
}

Client::Builder& Client::Builder::maxPoolSize(int const val) {
    ctx_.max_pool_size = val;
    return *this;
}

Client::Builder& Client::Builder::maxQueueSize(int const val) {
    ctx_.max_queue_size = val;
    return *this;
}

Client Client::Builder::build() {
    return Client{std::move(ctx_)};
}

}  // namespace postgres
