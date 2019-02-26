#include <postgres/internal/ConnectionPool.h>
#include <postgres/internal/Worker.h>

namespace postgres::internal {

ConnectionPool::ConnectionPool(std::shared_ptr<Context const> ctx)
    : ctx_{ctx}, chan_{std::make_shared<Channel>(ctx)} {
}

ConnectionPool::~ConnectionPool() noexcept {
    switch (ctx_->shutdownPolicy()) {
        case ShutdownPolicy::DROP: {
            chan_->drop();
            [[fallthrough]];
        }
        case ShutdownPolicy::GRACEFUL: {
            for (auto n = chan_.use_count() - 1; 0 < n; --n) {
                chan_->quit();
            }
            break;
        }
        case ShutdownPolicy::ABORT: {
            break;
        }
    }
}

void ConnectionPool::checkWorkers(Worker* const worker) {
    if (worker != nullptr) {
        worker->run();
        return;
    }

    if (static_cast<int>(workers_.size()) == ctx_->maxPoolSize()) {
        return;
    }

    auto new_worker = std::make_unique<internal::Worker>(ctx_, chan_);
    new_worker->run();
    workers_.push_back(std::move(new_worker));
}

}  // namespace postgres::internal
