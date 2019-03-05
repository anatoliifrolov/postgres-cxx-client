#include <gtest/gtest.h>
#include <postgres/internal/Dispatcher.h>
#include <postgres/internal/Worker.h>
#include <postgres/Context.h>
#include "ChannelFake.h"
#include "ChannelMock.h"

using testing::_;
using testing::Invoke;

namespace postgres::internal {

static void noop(Connection&) {
}

TEST(DispatcherTest, Reuse) {
    ChannelFake chan{};
    auto const  mock = std::make_shared<ChannelMock>();
    EXPECT_CALL(*mock, send(_)).WillOnce(Invoke(chan.sender(false)))
                               .WillOnce(Invoke(chan.sender(true)));
    EXPECT_CALL(*mock, receive(_)).Times(3).WillRepeatedly(Invoke(chan.receiver()));
    EXPECT_CALL(*mock, quit(1)).WillOnce(Invoke(chan.terminator()));
    EXPECT_CALL(*mock, recycle(_)).WillOnce(Invoke(chan.recycler()));
    Dispatcher disp{Context::Builder{}.maxConcurrency(2).share(), mock};
    disp.send<void>(noop).wait();
    disp.send<void>(noop).wait();
}

TEST(DispatcherTest, Drop) {
    ChannelFake chan{};
    auto const  mock = std::make_shared<ChannelMock>();
    EXPECT_CALL(*mock, send(_)).WillOnce(Invoke(chan.sender(false)));
    EXPECT_CALL(*mock, receive(_)).Times(2).WillRepeatedly(Invoke(chan.receiver()));
    EXPECT_CALL(*mock, drop()).Times(1);
    EXPECT_CALL(*mock, quit(1)).WillOnce(Invoke(chan.terminator()));
    EXPECT_CALL(*mock, recycle(_)).WillOnce(Invoke(chan.recycler()));
    Dispatcher disp{Context::Builder{}.maxConcurrency(2)
                                      .shutdownPolicy(ShutdownPolicy::DROP)
                                      .share(), mock};
    disp.send<void>(noop);
}

TEST(DispatcherTest, Recycle) {
    ChannelFake chan{};
    auto const  mock = std::make_shared<ChannelMock>();
    EXPECT_CALL(*mock, send(_)).Times(2).WillRepeatedly(Invoke(chan.sender(false)));
    EXPECT_CALL(*mock, receive(_)).Times(4).WillRepeatedly(Invoke(chan.receiver()));
    EXPECT_CALL(*mock, quit(1)).WillOnce(Invoke(chan.terminator()));
    EXPECT_CALL(*mock, recycle(_)).Times(2).WillRepeatedly(Invoke(chan.recycler()));
    Dispatcher disp{Context::Builder{}.maxConcurrency(2).share(), mock};
    disp.send<void>(noop).wait();
    chan.recycle();
    disp.send<void>(noop).wait();
}

TEST(DispatcherTest, Scale) {
    ChannelFake chan{};
    auto const  mock = std::make_shared<ChannelMock>();
    EXPECT_CALL(*mock, send(_)).Times(3).WillRepeatedly(Invoke(chan.sender(false)));
    EXPECT_CALL(*mock, receive(_)).Times(5).WillRepeatedly(Invoke(chan.receiver()));
    EXPECT_CALL(*mock, quit(2)).WillOnce(Invoke(chan.terminator()));
    EXPECT_CALL(*mock, recycle(_)).Times(2).WillRepeatedly(Invoke(chan.recycler()));
    Dispatcher disp{Context::Builder{}.maxConcurrency(2).share(), mock};
    disp.send<void>(noop);
    disp.send<void>(noop);
    disp.send<void>(noop).wait();
}

}  // namespace postgres::internal
