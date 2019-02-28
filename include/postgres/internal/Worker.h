#pragma once

#include <memory>
#include <thread>
#include <postgres/internal/Job.h>

namespace postgres {

class Context;

}  // namespace postgres

namespace postgres::internal {

class IChannel;

class Worker {
public:
    explicit Worker(std::shared_ptr<Context const> ctx, std::shared_ptr<IChannel> chan);
    Worker(Worker const& other) = delete;
    Worker& operator=(Worker const& other) = delete;
    Worker(Worker&& other) noexcept = delete;
    Worker& operator=(Worker&& other) noexcept = delete;
    ~Worker() noexcept;

    void run();

private:
    std::shared_ptr<Context const> ctx_;
    std::shared_ptr<IChannel>      chan_;
    Slot                           slot_;
    std::thread                    thread_;
};

}  // namespace postgres::internal
