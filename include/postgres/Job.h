#pragma once

#include <functional>

namespace postgres {

class Connection;
class Result;
class Status;

using Job = std::function<Status(Connection&)>;
using QueryJob = std::function<Result(Connection&)>;

}  // namespace postgres
