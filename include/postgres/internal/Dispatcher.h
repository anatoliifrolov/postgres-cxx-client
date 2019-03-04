#pragma once

#include <functional>
#include <future>
#include <memory>
#include <utility>
#include <vector>
#include <postgres/internal/IChannel.h>

namespace postgres {

class Connection;
class Context;

}  // namespace postgres

namespace postgres::internal {

class Worker;

class Dispatcher {
public:
    explicit Dispatcher(std::shared_ptr<Context const> ctx, std::shared_ptr<IChannel> chan);
    Dispatcher(Dispatcher const& other) = delete;
    Dispatcher& operator=(Dispatcher const& other) = delete;
    Dispatcher(Dispatcher&& other) noexcept = delete;
    Dispatcher& operator=(Dispatcher&& other) noexcept = delete;
    ~Dispatcher() noexcept;

    template <typename T>
    std::future<T> send(std::function<T(Connection&)> job) {
        auto task = std::make_shared<std::packaged_task<T(Connection&)>>(std::move(job));
        scale(chan_->send([task](Connection& conn) {
            (*task)(conn);
        }));
        return task->get_future();
    }

private:
    void scale(std::tuple<bool, Worker*> params);
    int size() const;

    std::shared_ptr<Context const>       ctx_;
    std::shared_ptr<IChannel>            chan_;
    std::vector<std::unique_ptr<Worker>> workers_;
};

}  // namespace postgres::internal
