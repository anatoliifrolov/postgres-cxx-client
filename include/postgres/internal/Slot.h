#pragma once

#include <condition_variable>
#include <mutex>
#include <postgres/internal/Job.h>

namespace postgres {

class Context;

}  // namespace postgres

namespace postgres::internal {

class Slot {
public:
    explicit Slot(std::shared_ptr<Context const> ctx);
    Slot(Slot const& other) = delete;
    Slot& operator=(Slot const& other) = delete;
    Slot(Slot&& other) noexcept = delete;
    Slot& operator=(Slot&& other) noexcept = delete;
    ~Slot() noexcept;

    void unlock(Job job);
    void fill(Job job);
    void wait();
    Job take();

private:
    std::shared_ptr<Context const> ctx_;
    Job                            job_;
    std::condition_variable        has_job_;
    std::mutex                     mtx_;
};

}  // namespace postgres::internal
