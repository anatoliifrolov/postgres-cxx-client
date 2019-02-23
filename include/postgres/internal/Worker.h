#pragma once

#include <thread>
#include <postgres/Connection.h>

namespace postgres {
namespace internal {

class Channel;
struct Context;

class Worker {
public:
    explicit Worker(Context const& ctx, Channel& ch);
    Worker(Worker const& other) = delete;
    Worker& operator=(Worker const& other) = delete;
    Worker(Worker&& other) noexcept;
    Worker& operator=(Worker&& other) noexcept;
    ~Worker() noexcept;

    void run();

private:
    Context const* ctx_;
    Channel* ch_;
    Connection  conn_;
    std::thread thread_;
};

}  // namespace internal
}  // namespace postgres
