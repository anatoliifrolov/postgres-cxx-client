#include <gtest/gtest.h>
#include <postgres/internal/Dispatcher.h>
#include <postgres/Context.h>

namespace postgres::internal {

void noop(Connection&) {
}

TEST(DispatcherTest, Result) {
    Dispatcher disp{};
    ASSERT_EQ(1, disp.send<int>([](Connection&) {
        return 1;
    }).get());
    ASSERT_THROW(disp.send<void>([](Connection&) {
        throw 1;
    }).get(), int);
}

TEST(DispatcherTest, Reuse) {
    Dispatcher disp{};
    ASSERT_EQ(0, disp.size());
    disp.send<void>(noop).wait();
    ASSERT_EQ(1, disp.size());
    disp.send<void>(noop).wait();
    ASSERT_EQ(1, disp.size());
}

TEST(DispatcherTest, Scale) {
    auto                    count = 0;
    std::mutex              mtx{};
    std::condition_variable signal{};

    auto const job = [&](Connection&) {
        std::unique_lock guard{mtx};
        ++count;
        signal.notify_all();
        signal.wait(guard, [&count] {
            return count == 2;
        });
    };

    Dispatcher disp{Context::Builder{}.maxConcurrency(2).build()};
    disp.send<void>(job);
    disp.send<void>(job).wait();
    ASSERT_EQ(2, disp.size());
}

}  // namespace postgres::internal
