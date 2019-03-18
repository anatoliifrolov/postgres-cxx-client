#include <vector>
#include <gtest/gtest.h>
#include <postgres/Client.h>
#include <postgres/Command.h>
#include <postgres/Connection.h>

namespace postgres {

TEST(ClientTest, Result) {
    Client cl{};
    ASSERT_TRUE(cl.exec([](Connection& conn) {
        return conn.execRaw("SELECT NULL");
    }).get().isOk());
    ASSERT_EQ(1, cl.query([](Connection& conn) {
        return conn.exec("SELECT 1::INT");
    }).get()[0][0].as<int32_t>());
    ASSERT_THROW(cl.exec([](Connection& conn) {
        return conn.exec("BAD").valid();
    }).get(), RuntimeError);
}

TEST(ClientTest, Load) {
    auto constexpr                   N = 64;
    Client                           cl{};
    std::vector<std::future<Result>> results{};
    results.reserve(N);

    for (auto i = 1; i <= N; ++i) {
        results.push_back(cl.query([i](Connection& conn) {
            return conn.exec(Command{"SELECT $1", i});
        }));
    }
    auto sum = 0;
    for (auto& res : results) {
        sum += res.get()[0][0].as<int32_t>();
    }
    ASSERT_EQ(2080, sum);
}

}  // namespace postgres
