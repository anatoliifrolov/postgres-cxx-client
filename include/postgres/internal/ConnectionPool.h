#pragma once

#include <functional>
#include <future>
#include <memory>
#include <utility>
#include <vector>
#include <postgres/internal/Channel.h>
#include <postgres/Context.h>

namespace postgres {

class Connection;

}  // namespace postgres

namespace postgres::internal {

class Worker;

class ConnectionPool {
public:
    explicit ConnectionPool(std::shared_ptr<Context const> ctx);
    ConnectionPool(ConnectionPool const& other) = delete;
    ConnectionPool& operator=(ConnectionPool const& other) = delete;
    ConnectionPool(ConnectionPool&& other) noexcept = delete;
    ConnectionPool& operator=(ConnectionPool&& other) noexcept = delete;
    ~ConnectionPool() noexcept;

    template <typename T>
    std::future<T> send(std::function<T(Connection&)> job) {
        // todo: cannot compile with just packaged_task or unique_ptr :(
        auto task = std::make_shared<std::packaged_task<T(Connection&)>>(std::move(job));
        auto fut  = task->get_future();
        Worker* worker = nullptr;
        if (chan_->send([task = std::move(task)](Connection& conn) mutable {
            (*task)(conn);
        }, ctx_->maxQueueSize(), worker)) {
            return fut;
        }

        checkWorkers(worker);
        return fut;
    }

private:
    void checkWorkers(Worker* worker);

    std::shared_ptr<Context const>       ctx_;
    std::shared_ptr<Channel>             chan_;
    std::vector<std::unique_ptr<Worker>> workers_;
};

}  // namespace postgres::internal
