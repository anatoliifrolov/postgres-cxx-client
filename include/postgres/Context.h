#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <postgres/Config.h>
#include <postgres/PreparingStatement.h>

namespace postgres {

class Connection;

enum class ShutdownPolicy {
    GRACEFUL,
    DROP,
    ABORT,
};

class Context {
public:
    class Builder;

    explicit Context();
    Context(Context const& other) = delete;
    Context& operator=(Context const& other) = delete;
    Context(Context&& other) noexcept;
    Context& operator=(Context&& other) noexcept;
    ~Context() noexcept;

    Connection connect() const;
    std::chrono::seconds idleTimeout() const;
    int maxPoolSize() const;
    int maxQueueSize() const;
    ShutdownPolicy shutdownPolicy() const;

private:
    Config                          cfg_;
    std::string                     uri_;
    std::vector<PreparingStatement> prep_statements_;
    std::chrono::seconds            idle_timeout_;
    int                             max_pool_size_;
    int                             max_queue_size_;
    ShutdownPolicy                  shut_policy_;
};

class Context::Builder {
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
    Builder& idleTimeout(std::chrono::seconds dur);
    Builder& maxPoolSize(int size);
    Builder& maxQueueSize(int size);
    Builder& shutdownPolicy(ShutdownPolicy pol);

    Context build();

private:
    Context ctx_;
};

}  // namespace postgres
