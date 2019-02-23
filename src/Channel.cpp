#include <postgres/internal/Channel.h>

#include <algorithm>
#include <postgres/Context.h>
#include <postgres/Error.h>

namespace postgres {
namespace internal {

Channel::Channel(std::shared_ptr<Context const> ctx)
    : ctx_{ctx} {
    slots_.reserve(ctx->maxPoolSize());
}

Channel::~Channel() noexcept = default;

bool Channel::send(Job job, size_t const lim) {
    std::lock_guard lock{mtx_};
    if (!slots_.empty()) {
        auto const slot = slots_.back();
        slots_.pop_back();
        slot->unlock(std::move(job));
        return true;
    }

    if (lim != 0) {
        _POSTGRES_CXX_ASSERT(queue_.size() < lim, "queue overflow");
    }

    queue_.push(std::move(job));
    return false;
}

bool Channel::fill(Slot& slot) {
    std::lock_guard lock{mtx_};
    if (!queue_.empty()) {
        slot.fill(std::move(queue_.back()));
        queue_.pop();
        return true;
    }

    slots_.push_back(&slot);
    if (slots_.size() < ctx_->maxPoolSize()) {
        return false;
    }

    std::sort(slots_.begin(), slots_.end());
    return false;
}

void Channel::drop() {
    std::lock_guard lock{mtx_};
    auto queue = std::move(queue_);
}

}  // namespace internal
}  // namespace postgres
