#include <postgres/Context.h>

#include <thread>
#include <postgres/Connection.h>
#include <postgres/Error.h>

namespace postgres {

Context::Context()
    : cfg_{Config::build()},
      max_idle_{0},
      max_concur_{static_cast<int>(std::thread::hardware_concurrency())},
      max_queue_{0},
      shut_pol_{ShutdownPolicy::GRACEFUL} {
}

Context::Context(Context&& other) noexcept = default;

Context& Context::operator=(Context&& other) noexcept = default;

Context::~Context() noexcept = default;

Connection Context::connect() const {
    auto conn = uri_.empty() ? Connection{cfg_} : Connection{uri_};
    conn.check();
    for (auto const& prep : preparings_) {
        conn.exec(prep).check();
    }
    return conn;
}

Context::Duration Context::idleTimeout() const {
    return max_idle_;
}

int Context::maxConcurrency() const {
    return max_concur_;
}

int Context::maxQueueSize() const {
    return max_queue_;
}

ShutdownPolicy Context::shutdownPolicy() const {
    return shut_pol_;
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

Context::Builder& Context::Builder::prepare(PrepareData prep) {
    ctx_.preparings_.push_back(std::move(prep));
    return *this;
}

Context::Builder& Context::Builder::idleTimeout(Context::Duration const val) {
    _POSTGRES_CXX_ASSERT(0 <= val.count(), "bad idle timeout: " << val.count());
    ctx_.max_idle_ = val;
    return *this;
}

Context::Builder& Context::Builder::maxConcurrency(int const val) {
    _POSTGRES_CXX_ASSERT(1 <= val, "bad concurrency: " << val);
    ctx_.max_concur_ = val;
    return *this;
}

Context::Builder& Context::Builder::maxQueueSize(int const val) {
    _POSTGRES_CXX_ASSERT(0 <= val, "bad queue size: " << val);
    ctx_.max_queue_ = val;
    return *this;
}

Context::Builder& Context::Builder::shutdownPolicy(ShutdownPolicy const val) {
    ctx_.shut_pol_ = val;
    return *this;
}

Context Context::Builder::build() {
    return std::move(ctx_);
}

std::shared_ptr<Context> Context::Builder::share() {
    return std::make_shared<Context>(build());
}

}  // namespace postgres
