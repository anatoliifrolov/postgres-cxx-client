#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <tuple>
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

    void quit(int count);
    std::tuple<bool, Worker*> send(Job job);
    void receive(Slot& slot);
    void recycle(Worker& worker);
    void drop();

private:
    std::tuple<bool, Worker*> send(Job job, int lim);

    std::shared_ptr<Context const> ctx_;
    std::queue<Job>                queue_;
    std::vector<Slot*>             slots_;
    std::vector<Worker*>           recreation_;
    std::mutex                     mtx_;
};

}  // namespace postgres::internal
