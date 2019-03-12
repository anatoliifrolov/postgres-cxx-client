#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <vector>
#include <postgres/internal/IChannel.h>

namespace postgres {

class Context;

}  // namespace postgres

namespace postgres::internal {

class Channel : public IChannel {
public:
    explicit Channel(std::shared_ptr<Context const> ctx);
    Channel(Channel const& other) = delete;
    Channel& operator=(Channel const& other) = delete;
    Channel(Channel&& other) noexcept = delete;
    Channel& operator=(Channel&& other) noexcept = delete;
    ~Channel() noexcept override;

    std::tuple<bool, Worker*> send(Job job) override;
    void receive(Slot& slot) override;
    void recycle(Worker& worker) override;
    void drop() override;
    void quit(int count) override;

private:
    std::tuple<bool, Worker*> send(Job job, int lim);

    std::shared_ptr<Context const> ctx_;
    std::queue<Job>                queue_;
    std::set<Slot*>                slots_;
    std::vector<Worker*>           recreation_;
    std::mutex                     mtx_;
};

}  // namespace postgres::internal
