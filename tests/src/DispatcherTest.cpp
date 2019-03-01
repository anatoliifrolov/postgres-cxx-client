#include <condition_variable>
#include <mutex>
#include <queue>
#include <gtest/gtest.h>
#include <postgres/internal/Channel.h>
#include <postgres/internal/Dispatcher.h>
#include <postgres/internal/Job.h>
#include <postgres/internal/Worker.h>
#include <postgres/Context.h>
#include "ChannelMock.h"

using testing::_;
using testing::Invoke;

namespace postgres::internal {

struct Queue {
    auto terminator() {
        return [this](int count) {
            std::lock_guard guard{mtx};
            while (0 < count--) {
                jobs.push(nullptr);
            }
            signal.notify_all();
        };
    }

    auto sender(bool const is_sent = false) {
        return [this, is_sent](Job job) {
            std::lock_guard guard{mtx};
            jobs.push(std::move(job));
            signal.notify_one();
            return std::make_tuple(is_sent, nullptr);
        };
    }

    auto receiver() {
        return [this](Slot& slot) {
            std::unique_lock guard{mtx};
            signal.wait(guard, [&] {
                return !jobs.empty();
            });
            slot.job.swap(jobs.front());
            jobs.pop();
        };
    }

    std::queue<Job>         jobs;
    std::condition_variable signal;
    std::mutex              mtx;
};

void noop(Connection&) {
}

TEST(DispatcherTest, Reuse) {
    Queue      queue{};
    auto const chan = std::make_shared<ChannelMock>();
    EXPECT_CALL(*chan, send(_)).WillOnce(Invoke(queue.sender()))
                               .WillOnce(Invoke(queue.sender(true)));
    EXPECT_CALL(*chan, receive(_)).WillRepeatedly(Invoke(queue.receiver()));
    EXPECT_CALL(*chan, quit(1)).WillOnce(Invoke(queue.terminator()));
    EXPECT_CALL(*chan, recycle(_)).Times(1);
    Dispatcher disp{std::make_shared<Context>(Context::Builder{}.maxConcurrency(2).build()), chan};
    disp.send<void>(noop).wait();
    disp.send<void>(noop).wait();
}

TEST(DispatcherTest, Scale) {
    Queue      queue{};
    auto const chan = std::make_shared<ChannelMock>();
    EXPECT_CALL(*chan, send(_)).WillRepeatedly(Invoke(queue.sender()));
    EXPECT_CALL(*chan, receive(_)).WillRepeatedly(Invoke(queue.receiver()));
    EXPECT_CALL(*chan, quit(2)).WillOnce(Invoke(queue.terminator()));
    EXPECT_CALL(*chan, recycle(_)).Times(2);
    Dispatcher disp{std::make_shared<Context>(Context::Builder{}.maxConcurrency(2).build()), chan};
    disp.send<void>(noop);
    disp.send<void>(noop);
    disp.send<void>(noop).wait();
}

}  // namespace postgres::internal
