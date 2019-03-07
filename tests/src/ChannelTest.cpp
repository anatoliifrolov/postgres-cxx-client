#include <future>
#include <vector>
#include <gtest/gtest.h>
#include <postgres/internal/Channel.h>
#include <postgres/internal/Worker.h>
#include <postgres/Context.h>
#include <postgres/Error.h>

using namespace std::chrono_literals;

namespace postgres::internal {

TEST(ChannelTest, Send) {
    auto const ctx  = Context::Builder{}.share();
    auto const chan = std::make_shared<Channel>(ctx);

    std::promise<int> prom{};
    auto const[is_sent, recycled] = chan->send([&prom](Connection&) {
        prom.set_value(1);
    });
    ASSERT_FALSE(is_sent);
    ASSERT_EQ(nullptr, recycled);

    Worker worker{ctx, chan};
    worker.run();
    ASSERT_EQ(1, prom.get_future().get());

    chan->quit(1);
}

TEST(ChannelTest, Graceful) {
    auto const ctx  = Context::Builder{}.maxConcurrency(1).share();
    auto const chan = std::make_shared<Channel>(ctx);

    std::vector<int> res{};

    for (auto i = 1; i <= 3; ++i) {
        chan->send([&res, i](Connection&) {
            res.push_back(i);
        });
    }
    chan->quit(1);

    Worker{ctx, chan}.run();
    ASSERT_EQ(1, res[0]);
    ASSERT_EQ(2, res[1]);
    ASSERT_EQ(3, res[2]);
}

TEST(ChannelTest, Drop) {
    auto const ctx  = Context::Builder{}.share();
    auto const chan = std::make_shared<Channel>(ctx);

    std::vector<int> res{};

    for (auto i = 1; i <= 3; ++i) {
        chan->send([&res, i](Connection&) {
            res.push_back(i);
        });
    }
    chan->drop();
    chan->quit(1);

    Worker{ctx, chan}.run();
    ASSERT_TRUE(res.empty());
}

TEST(ChannelTest, Recycle) {
    auto const ctx    = Context::Builder{}.share();
    auto const chan   = std::make_shared<Channel>(ctx);
    auto const worker = new Worker{ctx, chan};

    chan->quit(1);
    worker->run();
    delete worker;

    auto const[is_sent, recycled] = chan->send(nullptr);
    ASSERT_FALSE(is_sent);
    ASSERT_EQ(worker, recycled);
}

TEST(ChannelTest, Timeout) {
    auto const ctx    = Context::Builder{}.idleTimeout(1ns).share();
    auto const chan   = std::make_shared<Channel>(ctx);
    auto const worker = new Worker{ctx, chan};

    worker->run();
    delete worker;

    auto const[is_sent, recycled] = chan->send(nullptr);
    ASSERT_FALSE(is_sent);
    ASSERT_EQ(worker, recycled);
}

TEST(ChannelTest, Overflow) {
    auto const ctx  = Context::Builder{}.maxQueueSize(1).share();
    auto const chan = std::make_shared<Channel>(ctx);
    chan->send(nullptr);
    ASSERT_THROW(chan->send(nullptr), RuntimeError);
}

}  // namespace postgres::internal
