#pragma once

#include <tuple>
#include <postgres/internal/Job.h>

namespace postgres::internal {

class Worker;

class IChannel {
public:
    virtual ~IChannel() noexcept;

    virtual void quit(int count) = 0;
    virtual std::tuple<bool, Worker*> send(Job job) = 0;
    virtual void receive(Slot& slot) = 0;
    virtual void recycle(Worker& worker) = 0;
    virtual void drop() = 0;
};

}  // namespace postgres::internal
