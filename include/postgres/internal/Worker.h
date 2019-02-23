#pragma once

#include <thread>
#include <postgres/internal/Slot.h>

namespace postgres {

struct Context;

namespace internal {

class Channel;

class Worker {
public:
    explicit Worker(std::shared_ptr<Context const> ctx, std::shared_ptr<Channel> chan);
    Worker(Worker const& other) = delete;
    Worker& operator=(Worker const& other) = delete;
    Worker(Worker&& other) noexcept = delete;
    Worker& operator=(Worker&& other) noexcept = delete;
    ~Worker() noexcept;

    void run();

private:
    std::shared_ptr<Context const> ctx_;
    std::shared_ptr<Channel>       chan_;
    Slot                           slot_;
    std::thread                    thread_;
};

}  // namespace internal
}  // namespace postgres
