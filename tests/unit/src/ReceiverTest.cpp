#include <vector>
#include <gtest/gtest.h>
#include <postgres/Connection.h>
#include <postgres/PreparedCommand.h>
#include <postgres/PrepareData.h>
#include <postgres/Receiver.h>
#include <postgres/Result.h>
#include "Samples.h"

namespace postgres {

TEST(ReceiverTest, Ok) {
    auto rec = Connection{}.send("SELECT 1");
    auto res = rec.receive();
    ASSERT_TRUE(res.isOk());
    ASSERT_FALSE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = rec.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());
    ASSERT_THROW(res.begin(), LogicError);
}

TEST(ReceiverTest, Bad) {
    auto rec = Connection{}.send("BAD");
    ASSERT_THROW(rec.receive(), RuntimeError);

    auto res = rec.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());
}

TEST(ReceiverTest, Raw) {
    auto cons = Connection{}.sendRaw("SELECT 1");
    auto stat = cons.consume();
    ASSERT_TRUE(stat.isOk());
    ASSERT_FALSE(stat.isEmpty());
    ASSERT_FALSE(stat.isDone());

    stat = cons.consume();
    ASSERT_FALSE(stat.isOk());
    ASSERT_TRUE(stat.isEmpty());
    ASSERT_TRUE(stat.isDone());
}

TEST(ReceiverTest, RawBad) {
    auto cons = Connection{}.sendRaw("BAD");
    ASSERT_THROW(cons.consume(), RuntimeError);

    auto stat = cons.consume();
    ASSERT_FALSE(stat.isOk());
    ASSERT_TRUE(stat.isEmpty());
    ASSERT_TRUE(stat.isDone());
}

TEST(ReceiverTest, Prepare) {
    Connection conn{};
    auto       rec = conn.send(PrepareData{"select1", "SELECT 1"});
    auto       res = rec.receive();
    ASSERT_TRUE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = rec.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());

    rec = conn.send(PreparedCommand{"select1"});
    res = rec.receive();
    ASSERT_TRUE(res.isOk());
    ASSERT_FALSE(res.isEmpty());
    ASSERT_FALSE(res.isDone());

    res = rec.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());
}

TEST(ReceiverTest, PrepareBad) {
    Connection conn{};
    auto       rec = conn.send(PrepareData{"select1", "BAD"});
    ASSERT_THROW(rec.receive(), RuntimeError);

    auto res = rec.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());

    rec = conn.send(PreparedCommand{"select1"});
    ASSERT_THROW(rec.receive(), RuntimeError);

    res = rec.receive();
    ASSERT_FALSE(res.isOk());
    ASSERT_TRUE(res.isEmpty());
    ASSERT_TRUE(res.isDone());
}

TEST(ReceiverTest, Iter) {
    std::vector<int32_t> vals{};
    for (auto const& res : Connection{}.iter(SELECT_MULTI_ROW)) {
        if (res.isEmpty()) {
            continue;
        }

        vals.emplace_back();
        res[0][0] >> vals.back();
    }
    ASSERT_EQ(3u, vals.size());
    ASSERT_EQ(1, vals[0]);
    ASSERT_EQ(2, vals[1]);
    ASSERT_EQ(3, vals[2]);
}

TEST(ReceiverTest, IterEmpty) {
    auto n = 0;
    for (auto const& res : Connection{}.iter("SELECT 1 WHERE FALSE")) {
        if (res.isEmpty()) {
            continue;
        }

        ++n;
    }
    ASSERT_EQ(0, n);
}

TEST(ReceiverTest, Busy) {
    auto n   = 0;
    auto rec = Connection{}.send("SELECT 1");
    while (rec.isBusy()) {
        ++n;
    }
    ASSERT_LT(0, n);
}

TEST(ReceiverTest, Cleanup) {
    Connection conn{};
    conn.send("SELECT 1::INT");
    auto rec = conn.send("SELECT 2::INT");
    ASSERT_EQ(2, rec.receive()[0][0].as<int32_t>());
    ASSERT_TRUE(rec.receive().isDone());
}

TEST(ReceiverTest, Mix) {
    Connection conn{};
    auto       rec1 = conn.send("SELECT 1::INT");
    ASSERT_THROW(conn.send("SELECT 2::INT"), RuntimeError);
    ASSERT_EQ(1, rec1.receive()[0][0].as<int32_t>());
    ASSERT_TRUE(rec1.receive().isDone());
}

}  // namespace postgres
