#include <postgres/Context.h>

#include <thread>
#include <postgres/Connection.h>

namespace postgres {

Context::Context()
    : cfg_{Config::build()},
      max_pool_size_{std::thread::hardware_concurrency()},
      max_queue_size_{0},
      shutd_pol_{ShutdownPolicy::GRACEFUL} {
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

Context::Duration Context::idleTimeout() const {
    return idle_timeout_;
}

size_t Context::maxPoolSize() const {
    return max_pool_size_;
}

size_t Context::maxQueueSize() const {
    return max_queue_size_;
}

ShutdownPolicy Context::shutdownPolicy() const {
    return shutd_pol_;
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

Context::Builder& Context::Builder::idleTimeout(Context::Duration const dur) {
    ctx_.idle_timeout_ = dur;
    return *this;
}

Context::Builder& Context::Builder::maxPoolSize(size_t const size) {
    ctx_.max_pool_size_ = size;
    return *this;
}

Context::Builder& Context::Builder::maxQueueSize(size_t const size) {
    ctx_.max_queue_size_ = size;
    return *this;
}

Context Context::Builder::build() {
    return Context{std::move(ctx_)};
}

Context::Builder& Context::Builder::shutdownPolicy(ShutdownPolicy const pol) {
    ctx_.shutd_pol_ = pol;
    return *this;
}

}  // namespace postgres
