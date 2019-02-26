#include <postgres/internal/Channel.h>

#include <algorithm>
#include <utility>
#include <postgres/Context.h>
#include <postgres/Error.h>

namespace postgres::internal {

Channel::Channel(std::shared_ptr<Context const> ctx)
    : ctx_{std::move(ctx)} {
}

Channel::~Channel() noexcept = default;

void Channel::quit() {
    Worker* worker = nullptr;
    send(nullptr, 0, worker);
}

bool Channel::send(Job job, int const lim, Worker*& worker) {
    std::unique_lock c_guard{mtx_};
    if (!slots_.empty()) {
        auto const slot = slots_.back();
        slots_.pop_back();
        c_guard.unlock();

        std::lock_guard s_guard{slot->mtx};
        slot->job.swap(job);
        slot->has_job.notify_one();
        return true;
    }

    if (0 < lim) {
        _POSTGRES_CXX_ASSERT((static_cast<int>(queue_.size()) < lim),
                             "connection pool queue overflows");
    }

    queue_.push(std::move(job));
    if (workers_.empty()) {
        return false;
    }

    worker = workers_.back();
    workers_.pop_back();
    return false;
}

void Channel::receive(Slot& slot) {
    std::unique_lock c_guard{mtx_};
    if (!queue_.empty()) {
        slot.job.swap(queue_.back());
        queue_.pop();
        return;
    }

    slots_.push_back(&slot);
    std::sort(slots_.begin(), slots_.end());
    std::unique_lock s_guard{slot.mtx};
    c_guard.unlock();

    if (0 < ctx_->idleTimeout().count()) {
        slot.has_job.wait_for(s_guard, ctx_->idleTimeout());
        return;
    }

    slot.has_job.wait(s_guard);
}

void Channel::recycle(Worker& worker) {
    std::lock_guard guard{mtx_};
    workers_.push_back(&worker);
}

void Channel::drop() {
    std::lock_guard guard{mtx_};
    auto const      garbage = std::move(queue_);
}

}  // namespace postgres::internal
