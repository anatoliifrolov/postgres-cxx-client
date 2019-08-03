#include <gtest/gtest.h>
#include <postgres/internal/Worker.h>
#include <postgres/Connection.h>
#include <postgres/Context.h>
#include <postgres/Error.h>
#include "ChannelMock.h"

using testing::_;
using testing::Invoke;
using testing::Ref;

namespace postgres::internal {

TEST(WorkerTest, NoRun) {
    Worker{std::make_shared<Context>(), std::make_shared<ChannelMock>()};
}

TEST(WorkerTest, BadRun) {
    Worker w{std::make_shared<Context>(Context::Builder{}.uri("BAD").build()),
             std::make_shared<ChannelMock>()};
    ASSERT_THROW(w.run(), RuntimeError);
}

TEST(WorkerTest, Rerun) {
    auto const chan = std::make_shared<ChannelMock>();
    Worker     w{std::make_shared<Context>(), chan};
    EXPECT_CALL(*chan, receive(_)).Times(2);
    EXPECT_CALL(*chan, recycle(Ref(w))).Times(2);
    w.run();
    w.run();
}

TEST(WorkerTest, Job) {
    int32_t    res  = 0;
    auto const chan = std::make_shared<ChannelMock>();
    EXPECT_CALL(*chan, receive(_)).WillOnce(Invoke([&res](Slot& slot) {
        slot.job = [&res](Connection& conn) {
            conn.exec("SELECT 1::INT")[0][0] >> res;
        };
    })).WillOnce(Invoke([](Slot& slot) {
        slot.job = nullptr;
    }));
    EXPECT_CALL(*chan, recycle(_)).Times(1);

    Worker{std::make_shared<Context>(), chan}.run();
    ASSERT_EQ(1, res);
}

/*
TEST(WorkerTest, Break) {
    // todo
    // Unable to create a connection in a bad state anymore.
    // Need some other way to assure worker exits the loop on connection loss.
    auto const chan = std::make_shared<ChannelMock>();
    EXPECT_CALL(*chan, receive(_)).WillOnce(Invoke([](Slot& slot) {
        slot.job = [](Connection& conn) {
            conn = Connection{"BAD"};
        };
    }));
    EXPECT_CALL(*chan, recycle(_)).Times(1);
    Worker{std::make_shared<Context>(), chan}.run();
}
*/

}  // namespace postgres::internal
