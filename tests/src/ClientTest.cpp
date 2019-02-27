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

TEST(ClientTest, Reuse) {
    Client cl{};

    auto const job = [](Connection& conn) {
        std::stringstream strm{};
        strm << std::this_thread::get_id();
        return conn.exec(Command{"SELECT $1", strm.str()});
    };

    auto const id1 = cl.query(job).get()[0][0].as<std::string>();
    auto const id2 = cl.query(job).get()[0][0].as<std::string>();
    ASSERT_FALSE(id1.empty());
    ASSERT_EQ(id1, id2);
}

TEST(ClientTest, Scale) {
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
        return Status{nullptr};
    };

    Client cl{};
    auto   st1     = cl.exec(job);
    auto   st2     = cl.exec(job);
    st1.wait();
    st2.wait();
    ASSERT_EQ(2, count);
}

}  // namespace postgres
