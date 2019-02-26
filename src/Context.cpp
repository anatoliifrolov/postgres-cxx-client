#include <postgres/Context.h>

#include <thread>
#include <postgres/Connection.h>

namespace postgres {

Context::Context()
    : cfg_{Config::build()},
      max_pool_size_{static_cast<int>(std::thread::hardware_concurrency())},
      max_queue_size_{0},
      shut_policy_{ShutdownPolicy::GRACEFUL} {
}

Context::Context(Context&& other) noexcept = default;

Context& Context::operator=(Context&& other) noexcept = default;

Context::~Context() noexcept = default;

Connection Context::connect() const {
    auto conn = uri_.empty() ? Connection{cfg_} : Connection{uri_};
    conn.check();
    for (auto const& stmt : prep_statements_) {
        conn.exec(stmt).check();
    }
    return conn;
}

std::chrono::seconds Context::idleTimeout() const {
    return idle_timeout_;
}

int Context::maxPoolSize() const {
    return max_pool_size_;
}

int Context::maxQueueSize() const {
    return max_queue_size_;
}

ShutdownPolicy Context::shutdownPolicy() const {
    return shut_policy_;
}

Context::Builder::Builder() = default;

Context::Builder::Builder(Context::Builder&& other) noexcept = default;

Context::Builder& Context::Builder::operator=(Context::Builder&& other) noexcept = default;

Context::Builder::~Builder() noexcept = default;

Context::Builder& Context::Builder::config(Config cfg) {
    ctx_.cfg_ = std::move(cfg);
    return *this;
}

Context::Builder& Context::Builder::uri(std::string uri) {
    ctx_.uri_ = std::move(uri);
    return *this;
}

Context::Builder& Context::Builder::prepare(PreparingStatement stmt) {
    ctx_.prep_statements_.push_back(std::move(stmt));
    return *this;
}

Context::Builder& Context::Builder::idleTimeout(std::chrono::seconds const dur) {
    ctx_.idle_timeout_ = dur;
    return *this;
}

Context::Builder& Context::Builder::maxPoolSize(int const size) {
    ctx_.max_pool_size_ = size;
    return *this;
}

Context::Builder& Context::Builder::maxQueueSize(int const size) {
    ctx_.max_queue_size_ = size;
    return *this;
}

Context Context::Builder::build() {
    return Context{std::move(ctx_)};
}

Context::Builder& Context::Builder::shutdownPolicy(ShutdownPolicy const pol) {
    ctx_.shut_policy_ = pol;
    return *this;
}

}  // namespace postgres
