/// ### Getting started coding
///
/// The following example gives you basic understanding of how to use the library.
/// Each concept is explained in detail in corresponding section below.
/// ```
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <postgres/Postgres.h>

using postgres::Command;
using postgres::Connection;
using postgres::Time;

struct MyTable {
    int                                   id;
    std::string                           info;
    std::chrono::system_clock::time_point create_time;

    POSTGRES_CXX_TABLE("my_table", id, info, create_time);
};

void getStarted() {
    // Connect to the database.
    Connection conn{};

    // Create my_table.
    conn.create<MyTable>().check();

    auto now = std::chrono::system_clock::now();

    // Populate the table with data.
    std::vector<MyTable> data{{1, "foo", now},
                              {2, "bar", now},
                              {3, "baz", now}};
    conn.insert(data.begin(), data.end()).check();

    // Retrieve some data from the table.
    auto query = "SELECT info, create_time FROM my_table WHERE $1 < id";

    for (auto const& row : conn.exec(Command{query, 1}).valid()) {
        std::cout
            << row["create_time"].as<Time>().toString()
            << " "
            << row["info"].as<std::string>()
            << std::endl;
    }
}
/// ```
