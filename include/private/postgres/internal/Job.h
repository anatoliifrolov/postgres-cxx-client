#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>

namespace postgres {

class Connection;

}  // namespace postgres

namespace postgres::internal {

using Job = std::function<void(Connection&)>;

struct Slot {
    Job                     job;
    std::condition_variable signal;
    std::mutex              mtx;
};

}  // namespace postgres::internal
