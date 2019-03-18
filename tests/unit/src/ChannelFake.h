#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>
#include <postgres/internal/Job.h>

namespace postgres::internal {

class Worker;

class ChannelFake {
public:
    explicit ChannelFake();
    ChannelFake(ChannelFake const& other) = delete;
    ChannelFake& operator=(ChannelFake const& other) = delete;
    ChannelFake(ChannelFake&& other) noexcept = delete;
    ChannelFake& operator=(ChannelFake&& other) noexcept = delete;
    ~ChannelFake() noexcept;

    auto sender(bool const directly) {
        return [this, directly](Job job) {
            return send(std::move(job), directly);
        };
    }

    auto receiver() {
        return [this](Slot& slot) {
            receive(slot);
        };
    }

    auto recycler() {
        return [this](Worker& worker) {
            recycle(worker);
        };
    }

    auto terminator() {
        return [this](int const count) {
            terminate(count);
        };
    }

    void recycle();

private:
    std::tuple<bool, Worker*> send(Job job, bool directly);
    void receive(Slot& slot);
    void recycle(Worker& worker);
    void terminate(int count);
    void quit(int count);

    std::queue<Job>         queue_;
    std::condition_variable sig_queue_;
    std::condition_variable sig_recycle_;
    std::vector<Worker*>    recreation_;
    std::mutex              mtx_;
};

}  // namespace postgres::internal
