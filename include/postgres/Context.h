#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <postgres/Config.h>
#include <postgres/PrepareData.h>

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
    int maxConcurrency() const;
    int maxQueueSize() const;
    ShutdownPolicy shutdownPolicy() const;

private:
    Config                   cfg_;
    std::string              uri_;
    std::vector<PrepareData> preparings_;
    Duration                 max_idle_;
    int                      max_concur_;
    int                      max_queue_;
    ShutdownPolicy           shut_pol_;
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
    Builder& prepare(PrepareData prep);
    Builder& idleTimeout(Context::Duration val);
    Builder& maxConcurrency(int val);
    Builder& maxQueueSize(int val);
    Builder& shutdownPolicy(ShutdownPolicy val);

    Context build();
    std::shared_ptr<Context> share();

private:
    Context ctx_;
};

}  // namespace postgres
