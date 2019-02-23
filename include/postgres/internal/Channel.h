#pragma once

#include <mutex>
#include <vector>
#include <postgres/internal/Job.h>
#include <postgres/internal/Slot.h>

namespace postgres {

class Context;

namespace internal {

class Channel {
public:
    explicit Channel(std::shared_ptr<Context const> ctx);
    Channel(Channel const& other) = delete;
    Channel& operator=(Channel const& other) = delete;
    Channel(Channel&& other) noexcept = delete;
    Channel& operator=(Channel&& other) noexcept = delete;
    ~Channel() noexcept;

    bool send(Job job, size_t lim);
    bool fill(Slot& slot);
    void drop();

private:
    std::shared_ptr<Context const> ctx_;
    internal::Queue                queue_;
    std::vector<Slot*>             slots_;
    std::mutex                     mtx_;
};

}  // namespace internal
}  // namespace postgres
