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
    using Duration = std::chrono::high_resolution_clock::duration;

    explicit Context();
    Context(Context const& other) = delete;
    Context& operator=(Context const& other) = delete;
    Context(Context&& other) noexcept;
    Context& operator=(Context&& other) noexcept;
    ~Context() noexcept;

    Connection connect() const;
    Duration idleTimeout() const;
    size_t maxPoolSize() const;
    size_t maxQueueSize() const;
    ShutdownPolicy shutdownPolicy() const;

private:
    Config                          cfg_;
    std::string                     uri_;
    std::vector<PreparingStatement> prep_statements_;
    Duration                        idle_timeout_;
    size_t                          max_pool_size_;
    size_t                          max_queue_size_;
    ShutdownPolicy                  shutd_pol_;
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
    Builder& idleTimeout(Context::Duration dur);
    Builder& maxPoolSize(size_t size);
    Builder& maxQueueSize(size_t size);
    Builder& shutdownPolicy(ShutdownPolicy pol);

    Context build();

private:
    Context ctx_;
};

}  // namespace postgres
