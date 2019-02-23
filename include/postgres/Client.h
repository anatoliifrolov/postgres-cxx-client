#pragma once

#include <functional>
#include <future>
#include <memory>

namespace postgres::internal {

class ConnectionPool;

}  // namespace postgres::internal

namespace postgres {

class Connection;
class Context;
class Result;
class Status;

class Client {
public:
    explicit Client();
    explicit Client(Context ctx);
    Client(Client const& other) = delete;
    Client& operator=(Client const& other) = delete;
    Client(Client&& other) noexcept;
    Client& operator=(Client&& other) noexcept;
    ~Client() noexcept;

    std::future<Status> exec(std::function<Status(Connection&)> job);
    std::future<Result> query(std::function<Result(Connection&)> job);

private:
    using Impl = internal::ConnectionPool;

    std::unique_ptr<Impl> impl_;
};

}  // namespace postgres
