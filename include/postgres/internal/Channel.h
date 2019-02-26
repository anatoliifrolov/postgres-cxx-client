#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <postgres/internal/Job.h>

namespace postgres {

class Context;

}  // namespace postgres

namespace postgres::internal {

class Worker;

class Channel {
public:
    explicit Channel(std::shared_ptr<Context const> ctx);
    Channel(Channel const& other) = delete;
    Channel& operator=(Channel const& other) = delete;
    Channel(Channel&& other) noexcept = delete;
    Channel& operator=(Channel&& other) noexcept = delete;
    ~Channel() noexcept;

    void quit();
    bool send(Job job, int lim, Worker*& worker);
    void receive(Slot& slot);
    void recycle(Worker& worker);
    void drop();

private:
    std::shared_ptr<Context const> ctx_;
    internal::Queue                queue_;
    std::vector<Slot*>             slots_;
    std::vector<Worker*>           workers_;
    std::mutex                     mtx_;
};

}  // namespace postgres::internal
