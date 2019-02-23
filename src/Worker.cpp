#include <postgres/internal/Worker.h>

#include <postgres/internal/Channel.h>
#include <postgres/Connection.h>
#include <postgres/Context.h>

namespace postgres {
namespace internal {

Worker::Worker(std::shared_ptr<Context const> ctx, std::shared_ptr<Channel> chan)
    : ctx_{ctx}, chan_{std::move(chan)}, slot_{std::move(ctx)} {
}

Worker::~Worker() noexcept {
    if (ctx_->shutdownPolicy() != ShutdownPolicy::ABORT) {
        thread_.join();
    }
}

void Worker::run() {
    auto conn = ctx_->connect();

    thread_ = std::thread([this, conn = std::move(conn)]() mutable {
        while (true) {
            if (!chan_->fill(slot_)) {
                slot_.wait();
            }
            auto job = slot_.take();
            if (!job) {
                break;
            }
            job(conn);
        }
    });
}

}  // namespace internal
}  // namespace postgres
