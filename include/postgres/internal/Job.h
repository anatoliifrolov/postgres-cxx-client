#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

namespace postgres {

class Connection;

}  // namespace postgres

namespace postgres::internal {

using Job = std::function<void(Connection&)>;
using Queue = std::queue<Job>;

struct Slot {
    Job                     job;
    std::condition_variable has_job;
    std::mutex              mtx;
};

}  // namespace postgres::internal
