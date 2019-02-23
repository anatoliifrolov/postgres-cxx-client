#pragma once

#include <functional>
#include <queue>

namespace postgres {

class Connection;

namespace internal {

using Job = std::function<void(Connection&)>;
using Queue = std::queue<Job>;

}  // namespace internal
}  // namespace postgres
