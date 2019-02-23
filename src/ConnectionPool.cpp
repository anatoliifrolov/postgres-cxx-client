#include <postgres/internal/ConnectionPool.h>

#include <postgres/internal/Worker.h>
#include <postgres/Error.h>

namespace postgres::internal {

ConnectionPool::ConnectionPool(std::shared_ptr<Context const> ctx)
    : ctx_{ctx}, chan_{std::make_shared<Channel>(ctx)} {
    workers_.reserve(ctx->maxPoolSize());
}

ConnectionPool::~ConnectionPool() noexcept {
    shutdown_.store(true);
    switch (ctx_->shutdownPolicy()) {
        case ShutdownPolicy::DROP: {
            chan_->drop();
            [[fallthrough]];
        }
        case ShutdownPolicy::GRACEFUL: {
            for (auto i = 0u; i < workers_.size(); i++) {
                chan_->send(nullptr, 0);
            }
            break;
        }
        case ShutdownPolicy::ABORT: {
            break;
        }
    }
}

void ConnectionPool::checkShutdown() {
    _POSTGRES_CXX_ASSERT(!shutdown_, "client is shutting down");
}

void ConnectionPool::checkWorkers() {
    if (workers_.size() == ctx_->maxPoolSize()) {
        return;
    }

    auto worker = std::make_unique<internal::Worker>(ctx_, chan_);
    worker->run();
    workers_.push_back(std::move(worker));
}

}  // namespace postgres::internal
