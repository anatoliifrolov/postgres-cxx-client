/// ### Get started with connection pool
///
/// Here is an example demonstrating how to use a connection pool.
/// ```C++
#include <iostream>
#include <vector>
#include <postgres/Postgres.h>

using postgres::Client;
using postgres::Command;
using postgres::Connection;
using postgres::Result;

void getStartedPool() {
    // Create a connection pool.
    Client cl{};

    std::vector<std::future<Result>> results{};
    results.reserve(10);

    // Send queries to separate threads for execution.
    for (auto i = 0; i < 10; ++i) {
        results.push_back(cl.query([i](Connection& conn) {
            return conn.exec(Command{"SELECT $1::INT", i});
        }));
    }

    // Wait for results to be ready and then handle them.
    for (auto& res :results) {
        std::cout << res.get().valid()[0][0].as<int>() << std::endl;
    }
}
/// ```
