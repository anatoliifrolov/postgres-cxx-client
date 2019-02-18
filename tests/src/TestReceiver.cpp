#include <gtest/gtest.h>
#include <postgres/Command.h>
#include <postgres/Config.h>
#include <postgres/Connection.h>
#include <postgres/PrepareData.h>
#include <postgres/Receiver.h>
#include <postgres/Result.h>

namespace postgres {

TEST(TestReceiver, Exec) {
    Connection conn{Config::build()};
    auto rcvr = conn.execAsync(Command{"SELECT 1"});
    ASSERT_TRUE(rcvr.isOk());

    auto res = rcvr.receive();
    ASSERT_TRUE(res.isOk());
    ASSERT_FALSE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = rcvr.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());
}

TEST(TestReceiver, ExecBad) {
    Connection conn{Config::build()};
    auto rcvr = conn.execAsync(Command{"BAD"});
    ASSERT_TRUE(rcvr.isOk());

    auto res = rcvr.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = rcvr.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());
}

TEST(TestReceiver, Prepare) {
    Connection conn{Config::build()};
    auto rcvr = conn.prepareAsync(PrepareData{"select1", "SELECT 1"});
    ASSERT_TRUE(rcvr.isOk());

    auto res = rcvr.receive();
    ASSERT_TRUE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = rcvr.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());

    rcvr = conn.execPreparedAsync(Command{"select1"});
    ASSERT_TRUE(rcvr.isOk());

    res = rcvr.receive();
    ASSERT_TRUE(res.isOk());
    ASSERT_FALSE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = rcvr.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());
}

TEST(TestReceiver, PrepareAsyncBad) {
    Connection conn{Config::build()};
    auto rcvr = conn.prepareAsync(PrepareData{"select1", "BAD"});
    ASSERT_TRUE(rcvr.isOk());

    auto res = rcvr.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = rcvr.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());

    rcvr = conn.execPreparedAsync(Command{"select1"});
    ASSERT_TRUE(rcvr.isOk());

    res = rcvr.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = rcvr.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());
}

//TEST_F(TestReceiver, Busy) {
//    ASSERT_TRUE(conn_->send(Command{"SELECT 1::INTEGER, 2::INTEGER, 3::INTEGER"}));
//    while (conn_->isBusy()) {
//        std::this_thread::sleep_for(std::chrono::milliseconds{1});
//    }
//    auto res = conn_->receive();
//    ASSERT_EQ(1, (int) res[0][0]);
//    ASSERT_EQ(2, (int) res[0][1]);
//    ASSERT_EQ(3, (int) res[0][2]);
//    ASSERT_TRUE(conn_->receive().isDone());
//}
//
//TEST_F(TestReceiver, RowByRow) {
//    ASSERT_TRUE(conn_->exec(Command{"INSERT INTO test(int4) VALUES(1), (2), (3)"}));
//    ASSERT_TRUE(conn_->send(Command{"SELECT int4 FROM test"}, AsyncMode::SINGLE_ROW));
//    std::set<int> data{};
//
//    for (auto res = conn_->receive(); !res.isDone(); res = conn_->receive()) {
//        if (!res.empty()) {
//            ASSERT_EQ(1, res.size());
//            data.insert((int) res[0][0]);
//        }
//    }
//    ASSERT_EQ((std::set<int>{1, 2, 3}), data);
//}

}  // namespace postgres
