#include "ChannelFake.h"

namespace postgres::internal {

ChannelFake::ChannelFake() = default;

ChannelFake::~ChannelFake() noexcept = default;

std::tuple<bool, Worker*> ChannelFake::send(Job job, bool const directly) {
    std::lock_guard guard{mtx_};
    queue_.push(std::move(job));
    sig_queue_.notify_one();
    if (recreation_.empty()) {
        return {directly, nullptr};
    }

    auto const worker = recreation_.back();
    recreation_.pop_back();
    return {directly, worker};
}

void ChannelFake::receive(Slot& slot) {
    std::unique_lock guard{mtx_};
    sig_queue_.wait(guard, [&] {
        return !queue_.empty();
    });
    slot.job.swap(queue_.front());
    queue_.pop();
};

void ChannelFake::recycle(Worker& worker) {
    std::lock_guard guard{mtx_};
    recreation_.push_back(&worker);
    sig_recycle_.notify_one();
};

void ChannelFake::recycle() {
    std::unique_lock guard{mtx_};
    quit(1);
    sig_recycle_.wait(guard);
}

void ChannelFake::terminate(int const count) {
    std::lock_guard guard{mtx_};
    quit(count);
}

void ChannelFake::quit(int count) {
    while (0 < count--) {
        queue_.push(nullptr);
        sig_queue_.notify_one();
    }
}

}  // namespace postgres::internal
