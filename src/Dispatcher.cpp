#include <postgres/internal/Dispatcher.h>

#include <postgres/internal/Worker.h>
#include <postgres/Context.h>

namespace postgres::internal {

Dispatcher::Dispatcher()
    : Dispatcher{Context{}} {
}

Dispatcher::Dispatcher(Context ctx)
    : ctx_{std::make_shared<Context>(std::move(ctx))}, chan_{std::make_shared<Channel>(ctx_)} {
}

Dispatcher::~Dispatcher() noexcept {
    switch (ctx_->shutdownPolicy()) {
        case ShutdownPolicy::DROP: {
            chan_->drop();
            [[fallthrough]];
        }
        case ShutdownPolicy::GRACEFUL: {
            chan_->quit(size());
            break;
        }
        case ShutdownPolicy::ABORT: {
            break;
        }
    }
}

void Dispatcher::scale(Worker* const recycled) {
    if (recycled != nullptr) {
        recycled->run();
        return;
    }

    if (size() == ctx_->maxConcurrency()) {
        return;
    }

    auto worker = std::make_unique<internal::Worker>(ctx_, chan_);
    worker->run();
    workers_.push_back(std::move(worker));
}

int Dispatcher::size() const {
    return static_cast<int>(workers_.size());
}

}  // namespace postgres::internal
