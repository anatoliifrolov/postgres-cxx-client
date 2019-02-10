#include <string>
#include <chrono>
#include <ctime>
#include <vector>
#include <functional>
#include <thread>
#include <iostream>
#include <postgres/Config.h>
#include <postgres/Connection.h>
#include <postgres/Command.h>
#include <postgres/PreparedCommand.h>
#include <postgres/PrepareData.h>
#include <postgres/Client.h>
#include <postgres/Visitable.h>

using postgres::Client;
using postgres::Command;
using postgres::Config;
using postgres::Connection;
using postgres::PreparedCommand;
using postgres::PrepareData;

Config makeConfig() {
    return Config::Builder{}.dbname("PGDATABASE")   // The same as the user by default.
                            .user("PGUSER")   // The same as user running app by default.
                            .password("PGPASSWORD")   // Not used if empty.
                            .hostaddr("127.0.0.1")   // Localhost by default.
                            .port(5432)  // 5432 by default.
                            .set("application_name", "postgres-example")
                            .set("connect_timeout", 3)
                            .build();
}

static constexpr auto DROP_TABLE_SQL = R"(
DROP TABLE IF EXISTS example)";

static constexpr auto CREATE_TABLE_SQL = R"(
CREATE TABLE example(
    n INTEGER,
    f DOUBLE PRECISION,
    b BOOLEAN,
    s TEXT,
    t TIMESTAMP
))";

void makeTestTable(Connection& conn) {
    conn.execute(DROP_TABLE_SQL) && conn.execute(CREATE_TABLE_SQL);
}

void basicUsage(Connection& conn) {
    auto const res = conn.execute("SELECT 1");

    // Something went wrong:
    if (!res) {
        // Connection is broken:
        if (!conn.isOk()) {
            // Reestablish it using all the same parameters.
            conn.reset();
        }
        std::cerr << res.errorMessage() << std::endl;
        return;
    }

    // Another way to check the result:
    try {
        res.validate();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return;
    }

    if (!res.empty()) {
        // Process result...
    }
}

void parametrizedInsert(Connection& conn) {
    // Recommended way to pass statement parameters is using Command:
    conn.execute(Command{R"(INSERT INTO example(n, f, b, s, t) VALUES($1, $2, $3, $4, $5))",
                         1,
                         2.34,
                         true,
                         "T'EXT",  // No need for escape!
                         std::chrono::system_clock::now()});

    // Take parameters from range:
    std::vector<int> vals{1, 2, 3};
    conn.execute(Command{"INSERT INTO example(n) VALUES($1), ($2), ($3)",
                         vals.begin(),
                         vals.end()});
}

void insertTimestamps(Connection& conn) {
    // You can also insert timestamp with time zone specified.
    // Call postgres::makeTimestamp() with second parameter set to true for that.
    // But there would be no way to read it back using this library.
    conn.execute(Command{"INSERT INTO example(t) VALUES($1), ($2), ($3)",
                         std::chrono::system_clock::now(),  // Implicitly recognized.
                         postgres::makeTimestamp(time(nullptr)),  // Must be explicitly converted to timestamp.
                         postgres::makeTimestamp("2017-08-25T13:03:35")});
}

void nonCopyingInsert(Connection& conn) {
    // By default Command stores string parameters by reference.
    // Make sure that object is staying alive during execution.
    // Pass rvalue reference to force Command to store parameter internally.
    std::string s = "SOME TEXT THAT IS TO BE GONE OUT OF SCOPE";
    conn.execute(Command{"INSERT INTO example(s) VALUES($1)", std::move(s)});
}

void insertNULLs(Connection& conn, const int* const n_ptr) {
    conn.execute(Command{"INSERT INTO example(n) VALUES($1), ($2)",
                         nullptr,  // Will be inserted as NULL.
                         n_ptr  // Pointed value will be inserted if pointer is valid, or NULL otherwise.
    });
}

void insertSpecialType(Connection& conn) {
    // Most parameter types are automatically recognized by the library.
    // But you can tell parameter type explicitly if needed:
    conn.execute(Command{"INSERT INTO example(s) VALUES($1)",
                         postgres::bindOid("WITH SPECIAL OID", TEXTOID)});
}

struct Example {
    int                                   n;
    double                                f;
    bool                                  b;
    std::string                           s;
    std::chrono::system_clock::time_point t;

    POSTGRES_CXX_TABLE(Example, n, f, b, s, t)
};

void insertVisitable(Connection& conn) {
    // Visitable fields n and s will be bound to $1 and $2 respectively.
    Example v{};
    v.n = 1;
    v.f = 2.34;
    v.b = true;
    v.s = "VISITABLE";
    v.t = std::chrono::system_clock::now();
    conn.execute(Command{"INSERT INTO example(n, f, b, s, t) VALUES($1, $2, $3, $4, $5)", v});
}

void executePrepared(Connection& conn) {
    // PreparedCommand is exactly the same as plain Command,
    // but accepting prepared statement name instead of statement text.
    if (conn.execute(PrepareData{"insert_s", "INSERT INTO example(s) VALUES($1)"})) {
        conn.execute(PreparedCommand{"insert_s", "PREPARED"});
    }
}

void executeAsync(Connection& conn) {
    // send() does NOT block contrary to execute().
    conn.send("SELECT 1");
    // But result() DOES block.
    auto const res = conn.nextResult();
    // Process result...
    // You MUST ALWAYS read result until Result::isDone() returns true.
    while (!conn.nextResult().isDone()) {
    }
}

void executeAsyncNonBlocking(Connection& conn) {
    conn.send("SELECT 1");
    // Wait until result is ready:
    while (conn.isBusy()) {
        // Do some other stuff...
        std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
    // Guaranteed not to block here.
    auto const res = conn.nextResult();
    // Process result...
    while (!conn.nextResult().isDone()) {
    }
}

void executeAsyncRowByRow(Connection& conn) {
    // Rows of large result set could be obtained one by one as they are ready.
    conn.send("SELECT * FROM example", postgres::AsyncMode::SINGLE_ROW);
    for (auto res = conn.nextResult(); !res.isDone(); res = conn.nextResult()) {
        // Note that the last result will be empty while isDone() will be still false.
        if (!res.empty()) {
            // Process result...
        }
    }
}

void cancelAsync(Connection& conn) {
    conn.send("INSERT INTO example(s) VALUES('CANCELED')");
    // Just tries to cancel, does not guarantee to succeed.
    // Returns whether cancel request has been dispatched.
    conn.cancel();
    while (!conn.nextResult().isDone()) {
    }
}

void readResultIntoVariables(Connection& conn) {
    int                                   n;
    double                                f;
    bool                                  b;
    std::string                           s;
    time_t                                t;
    std::chrono::system_clock::time_point tp;
    int* p = &n;  // Possibly NULL values must be read into pointers.

    // Result stays valid event after connection was destroyed.
    auto const res = conn.execute(R"(SELECT
            1 AS n,
            2.34::REAL AS f,
            TRUE AS b,
            'TEXT' AS s,
            TIMESTAMP '2017-08-25T13:03:35' AS t,
            TIMESTAMP '2017-08-25T13:03:35.987654' AS tp,
            NULL AS p)");

    // You can obtain the result in the following way (recommended):
    res.front() >> n >> f >> b >> s >> t >> tp >> p;

    // ...if the result contained multiple rows that would make sense to iterate it:
    for (auto tuple : res) {
        tuple >> n >> f >> b >> s >> t >> tp >> p;
    }

    for (auto it = res.begin(); it != res.end(); ++it) {
        *it >> n >> f >> b >> s >> t >> tp >> p;
    }

    for (auto i = 0; i < res.size(); ++i) {
        res[i] >> n >> f >> b >> s >> t >> tp >> p;
    }

    // ...or by field names:
    res.front()["n"] >> n;
    res.front()["f"] >> f;
    res.front()["b"] >> b;
    res.front()["s"] >> s;
    res.front()["t"] >> t;
    res.front()["tp"] >> tp;
    res.front()["p"] >> p;

    // ...or by field indexes:
    res.front()[0] >> n;
    res.front()[1] >> f;
    res.front()[2] >> b;
    res.front()[3] >> s;
    res.front()[4] >> t;
    res.front()[5] >> tp;
    res.front()[6] >> p;

    // ...or by both tuple and field indexes:
    res[0][0] >> n;
    res[0][1] >> f;
    res[0][2] >> b;
    res[0][3] >> s;
    res[0][4] >> t;
    res[0][5] >> tp;
    res[0][6] >> p;

    std::cout
        << "n = "
        << n
        << ", f = "
        << f
        << ", b = "
        << b
        << ", s = "
        << s
        << ", t = "
        << t
        << ", tp = "
        << tp.time_since_epoch().count()
        << ", p = "
        << p
        << std::endl;
}

void passResultToFunction(Connection& conn) {
    auto const someFunc = [](const int n, const std::string s) {
        std::cout << "n = " << n << ", s = " << s << std::endl;
    };

    auto const res = conn.execute("SELECT 1, 'TEXT'");
    // Result fields are implicitly converted to function argument types:
    someFunc(res[0][0], res[0][1]);
}

void prepareClient(Client& client) {
    // Creates schema if not exists and sets it for current connection.
    // true as second argument tells to cache schema name to set it again after reconnect.
    client.setSchema("public", true);
    // Will be cached and automatically prepared again as well.
    client.prepare(PrepareData{"insert_s", "INSERT INTO example(s) VALUES($1)"}, true);
}

void executeTransaction(Client& client) {
    // Passing multiple commands to execute() will force client to
    // wrap them into BEGIN and COMMIT/ROLLBACK if no transaction is in progress already.
    client.execute(PreparedCommand{"insert_s", "PREPARED BY CLIENT"},
                   Command{"INSERT INTO example(s) VALUES($1)", "INSERTED BY CLIENT COMMAND"},
                   "INSERT INTO example(s) VALUES('INSERTED BY CLIENT')");
}

void executeTransactionBlock(Client& client) try {
    // postgres::Transaction is a small RAII helper class
    // served to rollback transaction in case of exception's been thrown.
    auto transaction = client.begin();
    client.execute("INSERT INTO example(s) VALUES('TRANSACTION')");
    // Do some stuff...
    // Still inside transaction. This will throw:
    client.execute("BAD STATEMENT");
    // Won't reach here because of exception.
    transaction.commit();
} catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
}

void insertVisitable(Client& client) {
    // Insert statement is generated by library for visitable structures.
    client.execute("DELETE FROM example");
    std::vector<Example> data{};

    data.emplace_back();
    data.back().n = 1;
    data.back().f = 1.11;
    data.back().b = true;
    data.back().s = "VISITABLE1";
    data.back().t = std::chrono::system_clock::now();

    data.emplace_back();
    data.back().n = 2;
    data.back().f = 2.22;
    data.back().b = true;
    data.back().s = "VISITABLE2";
    data.back().t = std::chrono::system_clock::now();

    data.emplace_back();
    data.back().n = 3;
    data.back().f = 3.33;
    data.back().b = true;
    data.back().s = "VISITABLE3";
    data.back().t = std::chrono::system_clock::now();

    client.insert(data[0]);
    client.insert(data.begin() + 1, data.end());
}

void selectVisitable(Client& client) {
    // Table name to select from is taken from Visitable::_POSTGRES_CXX_TABLE_NAME.
    // Select statement is generated by library.
    std::vector<Example> data{};
    client.select(data);
}

int main() {
    auto const cfg = Config{};

    // postgres::Connection is relatively low level.
    // Better use postgres::Client instead.
    Connection conn{cfg};

    makeTestTable(conn);
    basicUsage(conn);
    parametrizedInsert(conn);
    insertTimestamps(conn);
    nonCopyingInsert(conn);
    insertNULLs(conn, nullptr);
    insertSpecialType(conn);
    insertVisitable(conn);
    executePrepared(conn);
    executeAsync(conn);
    executeAsyncNonBlocking(conn);
    executeAsyncRowByRow(conn);
    cancelAsync(conn);
    readResultIntoVariables(conn);
    passResultToFunction(conn);

    Client client{cfg};
    prepareClient(client);
    executeTransaction(client);
    executeTransactionBlock(client);
    insertVisitable(client);
    selectVisitable(client);

    std::cout << "Passed!" << std::endl;
    return 0;
}
