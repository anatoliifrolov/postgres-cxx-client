#pragma once

#include <future>
#include <postgres/internal/Context.h>
#include <postgres/Job.h>

namespace postgres {

class Client {
public:
    class Builder;

    Client(Client const& other) = delete;
    Client& operator=(Client const& other) = delete;
    Client(Client&& other) noexcept;
    Client& operator=(Client&& other) noexcept;
    ~Client() noexcept;

    static Client build();

    std::future<Status> exec(Job job);
    std::future<Result> exec(QueryJob job);

private:
    explicit Client(internal::Context ctx);

    internal::Context ctx_;
};

class Client::Builder {
public:
    explicit Builder();
    Builder(Builder const& other) = delete;
    Builder& operator=(Builder const& other) = delete;
    Builder(Builder&& other) noexcept;
    Builder& operator=(Builder&& other) noexcept;
    ~Builder() noexcept;

    Builder& config(Config cfg);
    Builder& uri(std::string uri);
    Builder& prepare(PreparingStatement stmt);
    Builder& maxPoolSize(int val);
    Builder& maxQueueSize(int val);

    Client build();

private:
    internal::Context ctx_;
};

}  // namespace postgres
