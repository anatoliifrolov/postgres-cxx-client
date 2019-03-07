#include <gtest/gtest.h>
#include <postgres/Connection.h>
#include <postgres/Context.h>
#include <postgres/Error.h>
#include <postgres/PreparedCommand.h>
#include <postgres/PrepareData.h>
#include "Connect.h"

using namespace std::chrono_literals;

namespace postgres {

TEST(ContextTest, Default) {
    Context const ctx{};
    ASSERT_EQ(0, ctx.idleTimeout().count());
    ASSERT_LT(0, ctx.maxConcurrency());
    ASSERT_EQ(0, ctx.maxQueueSize());
    ASSERT_EQ(ShutdownPolicy::GRACEFUL, ctx.shutdownPolicy());
}

TEST(ContextTest, Values) {
    auto const ctx = Context::Builder{}.idleTimeout(1s)
                                       .maxConcurrency(2)
                                       .maxQueueSize(3)
                                       .shutdownPolicy(ShutdownPolicy::DROP)
                                       .build();
    ASSERT_EQ(1s, ctx.idleTimeout());
    ASSERT_EQ(2, ctx.maxConcurrency());
    ASSERT_EQ(3, ctx.maxQueueSize());
    ASSERT_EQ(ShutdownPolicy::DROP, ctx.shutdownPolicy());
}

TEST(ContextTest, Bad) {
    ASSERT_THROW(Context::Builder{}.idleTimeout(-1s).build(), LogicError);
    ASSERT_THROW(Context::Builder{}.maxConcurrency(-1).build(), LogicError);
    ASSERT_THROW(Context::Builder{}.maxConcurrency(0).build(), LogicError);
    ASSERT_THROW(Context::Builder{}.maxQueueSize(-1).build(), LogicError);
}

TEST(ContextTest, Connect) {
    ASSERT_TRUE(Context{}.connect().isOk());
    ASSERT_TRUE(Context::Builder{}.uri(CONNECT_URI).build().connect().isOk());
    ASSERT_THROW(Context::Builder{}.uri("BAD").build().connect(), RuntimeError);
}

TEST(ContextTest, Prepare) {
    ASSERT_TRUE(Context::Builder{}.prepare(PrepareData{"select1", "SELECT 1"})
                                  .build()
                                  .connect()
                                  .exec(PreparedCommand{"select1"})
                                  .isOk());
    ASSERT_THROW(Context::Builder{}.prepare(PrepareData{"bad", "BAD"}).build().connect(),
                 RuntimeError);
}

TEST(ContextTest, PrepareMulti) {
    auto conn = Context::Builder{}.prepare(PrepareData{"select1", "SELECT 1"})
                                  .prepare(PrepareData{"select2", "SELECT 2"})
                                  .build()
                                  .connect();
    ASSERT_TRUE(conn.exec(PreparedCommand{"select1"}).isOk());
    ASSERT_TRUE(conn.exec(PreparedCommand{"select2"}).isOk());
}

}  // namespace postgres
