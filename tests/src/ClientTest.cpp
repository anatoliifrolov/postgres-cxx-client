#include <condition_variable>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <gtest/gtest.h>
#include <postgres/Client.h>
#include <postgres/Command.h>
#include <postgres/Connection.h>
#include <postgres/Status.h>

namespace postgres {

TEST(ClientTest, Exec) {
    Client cl{};
    ASSERT_TRUE(cl.exec([](Connection& conn) {
        return conn.execRaw("SELECT NULL");
    }).get().isOk());
    ASSERT_EQ(1, cl.query([](Connection& conn) {
        return conn.exec("SELECT 1::INT");
    }).get()[0][0].as<int32_t>());
    ASSERT_THROW(cl.exec([](Connection& conn) {
        return conn.exec("BAD").valid();
    }).get(), Error);
}

}  // namespace postgres
