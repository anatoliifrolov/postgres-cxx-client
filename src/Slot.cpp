#include <postgres/internal/Slot.h>

#include <utility>
#include <postgres/Context.h>

namespace postgres::internal {

Slot::Slot(std::shared_ptr<Context const> ctx)
    : ctx_{std::move(ctx)} {
}

Slot::~Slot() noexcept = default;

void Slot::unlock(Job job) {
    fill(std::move(job));
    has_job_.notify_one();
}

void Slot::fill(Job job) {
    std::lock_guard lock{mtx_};
    job_ = std::move(job);
}

void Slot::wait() {
    std::unique_lock lock{mtx_};
    if (0 < ctx_->idleTimeout().count()) {
        has_job_.wait_for(lock, ctx_->idleTimeout());
    } else {
        has_job_.wait(lock);
    }
}

Job Slot::take() {
    std::lock_guard lock{mtx_};
    return std::move(job_);
}

}  // namespace postgres::internal
