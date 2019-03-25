# Modern C++ client for PostgreSQL

This library is a wrapper around libpq designed 
to make your work with Postgres easy and safe in a modern C++ way. 
Features:
* C++17.
* Minimal dependencies.
* Connection pool.
* Asynchronous and row-by-row modes.
* Statements generation.
* Prepared statements.
* Transactions.
* Passing arguments in binary format.
* Working with timestamps and NULLs.

## Table of Contents

* [Getting started](#getting-started)
  * [CMake subproject](#cmake-subproject)
  * [Prebuilt library](#prebuilt-library)
  * [Running the tests](#running-the-tests)
* [License](#license)
* [Usage](#usage)
  * [Get started with connection](#get-started-with-connection)
  * [Get started with connection pool](#get-started-with-connection-pool)
  * [What to include](#what-to-include)
  * [Configuring](#configuring)
  * [Error handling](#error-handling)
  * [Statement execution](#statement-execution)
  * [Prepared statements](#prepared-statements)
  * [Multiple statements in one](#multiple-statements-in-one)
  * [Transactions](#transactions)
  * [Reading the result](#reading-the-result)
  * [Escaping](#escaping)
  * [Asynchronous interface](#asynchronous-interface)
  * [Generating statements](#generating-statements)
  * [Connection pool](#connection-pool)

<a name="getting-started"/>

## Getting started

Prerequisites:
* CMake 3.8 or newer.
* A C++17-compliant compiler.
* libpq-dev and postgresql-server-dev-all.
* Google Test (only to run the tests).

The project is built and tested using GCC 7.3 and Clang 6.0 on a machine running Linux.
As there is no platform specific code, it should also work on the other platforms as well,
but the author cannot guarantee this.

You can integrate the library into your project in two ways:
* As a CMake subproject.
* As a prebuilt library.

If you're experienced with CMake you probably already know what to do.
Anyway, lets go through the process step by step.
First we'll create a new CMake-based project which will serve us as an example.
Initially the project will have the following structure:
```
.
├── CMakeLists.txt
└── main.cpp
```
Lets fill out the CMakeLists.txt with:
```cmake
cmake_minimum_required(VERSION 3.8)
project(MyProject)

add_executable(MyProject main.cpp)
```
and write some code using the library in the main.cpp:
```cpp
#include <postgres/Connection.h>

int main() {
    return postgres::Connection::ping();
}
```
Now we have two ways to make the project compile as mentioned a bit earlier.
Lets consider each of them in turn.

<a name="cmake-subproject"/>

### CMake subproject

We have to bring the PostgreSQL client library files into our project somehow.
How to achieve this is up to you.
We could just download the repository or use a package manager,
but we'll utilize git submodules for this purpose:
```bash
$ git init
$ git submodule add \
    https://github.com/anatoliifrolov/postgres-cxx-client.git \
    ./deps/postgres-cxx-client/
```
Now we're able to add the library to our project's dependencies in the CMakeLists.txt:
```cmake
cmake_minimum_required(VERSION 3.8)
project(MyProject)

add_executable(MyProject main.cpp)

add_subdirectory(deps/postgres-cxx-client)
target_link_libraries(MyProject PostgresCxxClient::PostgresCxxClient)
```  
There are two new lines at the bottom doing the job.
And that's it, our project is ready to run.

<a name="prebuilt-library"/>

### Prebuilt library

Lets say we keep libraries in `~/lib/` directory.
Then the steps are as follows:
```bash
$ cd ~/lib/
$ git clone https://github.com/anatoliifrolov/postgres-cxx-client.git
$ cd postgres-cxx-client/
$ cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=~/lib/ \
    -B./build/release/ \
    -H.
$ cmake --build ./build/release/ --target install
```
Next we'll modify the CMakeLists.txt to look like:
```cmake
cmake_minimum_required(VERSION 3.8)
project(MyProject)

add_executable(MyProject main.cpp)

find_package(PostgresCxxClient)
target_link_libraries(MyProject PostgresCxxClient::PostgresCxxClient)
```
What's left is just to tell CMake where to lookup for the libraries
using the option `-DCMAKE_PREFIX_PATH=~/lib/`.
After this step our project should've become able to compile and run.

<a name="running-the-tests"/>

### Running the tests

To run the tests locally, create a database and a role both with a name "cxx_client".
Set the password "cxx_client" for the role and make the role be owner of the database.
How to do that is beyond the scope of this document.
Make sure that PostgreSQL server is up and ready to accept connections.
Then go to the library directory and from the command line type the following:
```bash
$ git submodule update --init --recursive
$ cmake -DPOSTGRES_CXX_BUILD_TESTS=ON -B./build/ -H.
$ cmake --build ./build/
$ cd ./build/
$ PGUSER=cxx_client PGPASSWORD=cxx_client PGDATABASE=cxx_client ctest -V
```
Pass any additional parameters you need to the ctest.
For instance, you may have to specify the database address with `PGHOST` or `PGHOSTADDR` variables.
On success you should see something similar to:
```
1/1 Test #1: PostgresCxxClientTest ............   Passed    0.79 sec
100% tests passed, 0 tests failed out of 1
Total Test time (real) =   0.79 sec
```

<a name="license"/>

## License

This project is licensed under the MIT License - see the LICENSE file for details.

<a name="usage"/>

## Usage

This section shows how to actually write code using the library.
All the examples are built and run as part of CI process and are guaranteed to work.

<a name="get-started-with-connection"/>

### Get started with connection

The following example gives you a basic idea of how to use the library.
Each feature is explained in detail in corresponding section below.
```
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
    // Connect to a database.
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
```

<a name="get-started-with-connection-pool"/>

### Get started with connection pool

Here is one more example to get you started.
At this time how to use a connection pool is demonstrated.
```
#include <iostream>
#include <vector>
#include <postgres/Postgres.h>

using postgres::Client;
using postgres::Command;
using postgres::Connection;
using postgres::Result;

void getStartedPool() {
    // Create a connection pool.
    Client client{};

    std::vector<std::future<Result>> results{};
    results.reserve(10);

    // Send queries to separate threads for execution.
    for (auto i = 0; i < 10; ++i) {
        results.push_back(client.query([i](Connection& conn) {
            return conn.exec(Command{"SELECT $1::INT", i});
        }));
    }

    // Wait for results to be ready and then handle them.
    for (auto& res :results) {
        std::cout << res.get().valid()[0][0].as<int>() << std::endl;
    }
}
```

<a name="what-to-include"/>

### What to include

The library provides an all-in-one header file `#include <postgres/Postgres.h>`,
the one with forward declarations `#include <postgres/Fwd.h>`,
and also every class that is a part of a public API has its own header which you can include.
It is strongly discouraged to declare any of the library types in your project code.
Include "Fwd.h" instead when you need just a declaration, say in a function signature.
If compilation time is not a paramount concern use "Postgres.h",
otherwise include only needed files from "postgres" directory.
The examples in this document include "Postgres.h" for brevity.

<a name="configuring"/>

### Configuring

You can find comprehensive description of Postgres configuration options
in the official libpq documentation at https://www.postgresql.org/docs/11/libpq-connect.html.
Here we will focus on an interface the library provides
to make it more convenient configuring a database connection.

Postgres has default values for all of its configuration parameter.
For instance, username defaults to the operating system name of the user running the app,
and database name is the same as the user name.
There are several ways to override the defaults:
- environment variables;
- connection string;
- URL;
- configuration builder.

Lets consider each one in turn with the examples.
```
void config() {
    Connection conn{};
    conn.check();
}
```
Here we connect to a database using the default values and environment variables.
This is a good choise to pass sensitive information like passwords.
For example, the library is tested assuming that
PGUSER, PGPASSWORD and PGDATABASE variables are set.

Alternatively we can use a connection string:
```
void configStr() {
    Connection conn{"user=cxx_client password=cxx_client dbname=cxx_client"};
    conn.check();
}
```
...or URL:
```
void configUrl() {
    Connection conn{"postgresql://cxx_client:cxx_client@/cxx_client"};
    conn.check();
}
```
And the last one approach is to exploit a configuration builder:
```
using postgres::Config;

void configBuilder() {
    Connection conn{Config::Builder{}.user("cxx_client")
                                     .password("cxx_client")
                                     .dbname("cxx_client")
                                     .build()};
    conn.check();
}
```
The `Config::Builder` provides setter methods for all parameters
available (and not deprecated) at the moment of writing.
Method names are in *snake_case* to exactly mirror corresponding parameter names.
There are quite a few options, e.g:
```
using namespace std::chrono_literals;
using postgres::SslMode;

void configBuilderExtra() {
    Connection conn{Config::Builder{}.application_name("APP")
                                     .keepalives(true)
                                     .keepalives_count(2)
                                     .keepalives_idle(3min)
                                     .sslmode(SslMode::DISABLE)
                                     .build()};
    conn.check();
}
```
We can also do the same thing with general purpose setters of the `Config::Builder`,
but their use is not recommended unless there is some brand new parameter
that has not yet been supported by the library.
Just in case:
```
void configBuilderManual() {
    Connection conn{Config::Builder{}.set("application_name", "APP")
                                     .enable("keepalives", true)
                                     .setNumber("keepalives_count", 2)
                                     .setInterval("keepalives_idle", 3min)
                                     .set("sslmode", "disable")
                                     .build()};
    conn.check();
}
```

<a name="error-handling"/>

### Error handling

One of the library goals was to eliminate some sorts of bugs by design and at compile time,
but of course runtime errors are unavoidable.
Most of the time you're given two ways to deal with them
depending on your attitude to exceptions.
Lets discuss exceptions first.

An exception classes hierarchy consists of a base class `postgres::Error`
and two classes derived from it: `postgres::LogicError` and `postgres::RuntimeError`.
The `Error` in turn is a child of `std::exception`.
The `LogicError` is triggered to indicate a bug in your code
such as trying to access a row that is out of bounds or misusing the library in some other way.
Runtime errors, e.g. a connection that's got broken,
don't usually cause an exception to be thrown unless you explicitly tell to do that.

Many library types provide two methods to check their status: `isOk()`
returning a boolean value and `check()` throwing an exception.
It is a common pattern used throughout the library, so remember it.
Also some types provide a method `valid()` which can throw an exception as well.
This one is kind of syntactic sugar and only works in chained method calls.
You will encounter it in a lot of examples.
The next section gives some more information on `valid()`.

Lets demonstrate the described behaviour using the `Connection` class
which we're now should be familiar with:
```
void connect() {
    Connection conn{};
    if (conn.isOk()) {
        std::cout << "Connected to a database" << std::endl;
    } else {
        std::cerr << "Fail to connect to a database: " << conn.message() << std::endl;
    }
}
```
And an exception-aware example:
```
using postgres::Error;

void connectCheck() {
    try {
        Connection conn{};
        conn.check();
    } catch (Error const& err) {
        std::cerr << err.what() << std::endl;
    }
}
```
Some errors might stem from a connection loss.
Once established a connection can be easily reset without the need to reconfigure it anew.
But all the connection state is gone including prepared statements, current schema, etc.
```
void connectReset(Connection& conn) {
    if (!conn.isOk()) {
        conn.reset();
    }
}
```

<a name="statement-execution"/>

### Statement execution

No that we've learned how to connect to a database lets execute some SQL-statements:
```
void exec(Connection& conn) {
    auto const res = conn.exec("SELECT 1").valid();
    // Handle the result...
}
```
The `exec()` returns an object of type `Result`.
The result is completely detached from the connection,
meaning it is safe to use it even after the connection has been closed.

Next we check that the execution has succeeded calling the `valid()` method,
which just forwards the result if it is in a good state,
and throws an exception otherwise.
This is similar to calling `unwrap()`, if you're familiar with Rust.

As stated above you're able to avoid exceptions if you'd like:
```
void execNoexcept(Connection& conn) {
    auto const res = conn.exec("BAD SQL");
    if (!res.isOk()) {
        std::cerr << res.message() << std::endl;
        return;
    }
}
```
It is often needed to parametrize a statement with values computed at runtime.
You have an option to embed those values directly into the statement text,
but it is a bad choise for several reasons:
- your app could get vulnerable to an SQL-injection attack;
- you have to deal with escaping;
- data is passed as text instead of binary format.

The library provides a better solution:
```
using postgres::Command;

void args(Connection& conn) {
    conn.exec(Command{"SELECT $1, $2", 42, "foo"}).check();
}
```
Under the hood argument types are passed to Postgres along with their values.
The `Command` automatically detects those types, but sometimes you have to be explicit.
In the example below if we hadn't specified the type of the argument
it would've been guessed to be plain text instead of json:
```
using postgres::bindOid;

void argsOid(Connection& conn) {
    auto const json = R"({"foo": "bar"})";
    conn.exec(Command{"SELECT $1", bindOid(json, JSONOID)}).check();
}
```
If there are arguments possibly having NULL values, use pointers or `std::optional` type.
In the following example both `ptr` and `opt` will be treated as NULLs:
```
void argsNull(Connection& conn) {
    int* ptr = nullptr;
    std::optional<int> opt;
    conn.exec(Command{"SELECT $1, $2", ptr, opt}).check();
}
```
The `Command` stores all the arguments into its internal buffer.
But there are cases when it is desirable to avoid copying, e.g. for a large piece of text.
This can be achieved by passing pointer to underlying C-style string
or by using `std::string_view`, but keep an eye on lifetimes.
The same is true for statements as well.
The both ways are shown below:
```
void argsLarge(Connection& conn) {
    std::string      text = "SOME VERY LONG TEXT...";
    std::string_view view = text;
    conn.exec(Command{"SELECT $1, $2", text.data(), view}).check();
}
```
That's how you can pass arguments stored in a container:
```
void argsRange(Connection& conn) {
    std::vector<int> args{1, 2, 3};
    conn.exec(Command{"SELECT $1, $2, $3", std::pair{args.begin(), args.end()}}).check();
}
```
Also there is an ability to add arguments afterwards:
```
void argsAfter(Connection& conn) {
    Command cmd{"SELECT $1, $2"};
    cmd << 42 << "foo";
    conn.exec(cmd).check();
}
```
And a final note about timestamps.
The recommended way is to use a database type called `TIMESTAMP`,
which represents a number of microseconds since Postgres epoch in UTC.
Instances of `std::chrono::system_clock::time_point` are easily converted to that type
and are accepted by the `Command` as arguments.
Of course you can work with timestamps that include time zone information as well,
but the library won't help you here, just pass them as strings.
Well, almost won't help - actually you can preserve your local time zone:
```
using postgres::Time;

void argsTime(Connection& conn) {
    auto now = std::chrono::system_clock::now();
    conn.exec(Command{"SELECT $1", Time{now, true}}).check();
}
```

<a name="prepared-statements"/>

### Prepared statements

Using prepared statements is quite trivial.
The first step is to prepare statement.
You have to specify statement name, statement body and argument types if present.
Then you can use the name to actually execute the statement and bind argument values.
Consider an example:
```
using postgres::PreparedCommand;
using postgres::PrepareData;

void prepare(Connection& conn) {
    conn.exec(PrepareData{"my_select", "SELECT $1", {INT4OID}}).check();
    conn.exec(PreparedCommand{"my_select", 123}).check();
}
```
Beware that the `Connection` is intentionally just a thin wrapper around native libpq handle
and doesn't keep any additional state.
Consequently, statements must be prepared again every time a connection's been reestablished.
Also using PgBouncer can lead to errors depending on its configuration.
If you're certain you've successfully prepared a statement and your code is correct,
but Postgres complains that the prepared statement doesn't exist,
then setting `pool_mode=session` in pgbouncer.ini is likely to solve the problem.

<a name="multiple-statements-in-one"/>

### Multiple statements in one

The `exec()` method described earlier allows to execute only one statement at a time,
which means that the following is a runtime error:
```
void execMultiBad(Connection& conn) {
    try {
        conn.exec("SELECT 1; SELECT 2").check();
    } catch (Error const& err) {
    }
}
```
But what if we have a migration file with plenty of statements
which we want to apply all at once?
Or, generalizing the problem, just want to join several statements into one for some reason?
Here is a solution:
```
void execMultiOk(Connection& conn) {
    conn.execRaw("SELECT 1; SELECT 2").check();
}
```
That's not an error anymore, but there are a couple of limitations.
The first one is that there is technically no way to pass arguments, only a statement.
Moreover you are not allowed to obtain data.
Don't be confused by the example - it is quite silly and just for demonstration purposes,
normally there won't be selects.
The second limitation is due to the library sends and receives arguments in binary format,
but when multiple statements are passed there is no way to tell Postgres to enable binary mode.
Also when a select statement is embedded somewhere between the other statements,
it is impossible to get the selected data
because only the result of the last statement is returned from a database.
Therefore it was decided to completely disable data read and avoid aforementioned issues.

<a name="transactions"/>

### Transactions

Each statement is executed as a separate transaction.
Multiple statements separated with semicolons as described in the previous section
are a single transaction as well.
There are two more apporaches to treat multiple statements as a transaction.
Lets start with the simplest one:
```
void transact(Connection& conn) {
    conn.transact("SELECT 1",
                  Command{"SELECT $1", 2},
                  PreparedCommand{"my_select", 3},
                  PrepareData{"my_select2", "SELECT $1"}).check();
}
```
The `transact()` accepts anything the `exec()` does:
strings, `Command`s, `PreparedCommand`s and `PrepareData` in any combination.
Either all of them succeed or none have any effect.
Again the example is a bit ridiculous, but imagine statements to be more meaningful,
for instance inserting data to two different tables when one insert without the other
would leave a system in inconsistent state.

The second way gives more fine-grained control over transaction execution:
```
void transactManual(Connection& conn) {
    auto tx = conn.begin().valid();
    conn.exec("SELECT 1").check();
    conn.exec("SELECT 2").check();
    tx.commit().check();
}
```
This way allows to put some logic between statement execution
and build more complex and flexible transactions.
Please don't forget to check that the `begin()` has succeeded and to commit.
Also consider the possibility of commit operation itself to fail.
When transaction handle goes out of scope it rollbacks the transaction
unless it has been explicitly commited already.

<a name="reading-the-result"/>

### Reading the result

Now it's time to talk about queries and how to access their results.
As mentioned above the `exec()` method returns an object of type `Result`.
Iterating over it will produce a `Row` instance on each iteration.
The 'Row' in turn consists of a number of `Field`s accessible by their index or name.
Finally you can read the value of the `Field` into a new variable or an existing one.
That is a bit less verbose expressed in code:
```
void result(Connection& conn) {
    for (auto const& row : conn.exec("SELECT 'foo' AS foo, 'bar' AS bar").valid()) {
        std::cout
            << row["foo"].as<std::string>()
            << " "
            << row["bar"].as<std::string>()
            << std::endl;
    }
}
```
Now lets store the same values into variables:
```
void resultVars(Connection& conn) {
    std::string foo, bar;

    auto const res = conn.exec("SELECT 'foo' AS foo, 'bar' AS bar").valid();
    if (res.isEmpty()) {
        // Normally this case should be covered as well...
    }

    res[0]["foo"] >> foo;
    res[0]["bar"] >> bar;

    std::cout << foo << " " << bar << std::endl;
}
```
In the last example it was evident that the result could not be empty.
In practice you should usually make a check before trying to access the data
or you will end up with a chance of going out of bounds.
Iterating eliminates this risk and therefore is safer and preferrable.

Similarly to the `Command` NULLs are represented with pointers or `std::optional`:
```
void resultNull(Connection& conn) {
    auto const res = conn.exec("SELECT NULL::TEXT").valid();
    auto const fld = res[0][0];

    // Bad idea.
    std::string s;
    try {
        fld >> s;
    } catch (Error const& err) {
    }

    // Ok.
    auto opt = fld.as<std::optional<std::string>>();

    // Also ok.
    auto ptr = &s;
    fld >> ptr;
}
```
You can cast the field to arithmetic type, but the rules are quite strict.
In particular the following is prohibited:
- loss of precision (casting from floating point value to integral one and vice versa);
- narrowing (casting larger type to smaller);
- underflow (reading negative values into variables of unsigned types).

Lets look how those three cases may appear in code:
```
void resultBadCast(Connection& conn) {
    auto const res = conn.exec("SELECT -1::BIGINT").valid();
    auto const fld = res[0][0];

    try {
        // Loss of precision.
        fld.as<double>();

        // Narrowing.
        fld.as<int32_t>();

        // Underflow.
        fld.as<uint64_t>();
    } catch (Error const& err) {
    }
}
```
Also the library is able to read timestamps without time zones:
```
void resultTime(Connection& conn) {
    auto const res = conn.exec("SELECT '2017-08-25T13:03:35'::TIMESTAMP").valid();
    auto const fld = res[0][0];

    // Modern C++ way.
    fld.as<std::chrono::system_clock::time_point>();

    // Getting time_t.
    fld.as<Time>().toUnix();
}
```
Timestamps with time zone have to be converted to `TEXT` and then read into `std::string`:
```
void resultTimeZone(Connection& conn) {
    auto const res = conn.exec("SELECT now()::TEXT").valid();
    auto const fld = res[0][0];

    // Prints something like '2019-03-21 12:58:13.256812+03'.
    std::cout << fld.as<std::string>() << std::endl;
}
```
And a small caveat about `extract(EPOCH FROM ...`-like statements.
Working with such a statement be aware that it yields the result of type `DOUBLE PRECISION`:
```
void resultExtractEpoch(Connection& conn) {
    auto const res = conn.exec("SELECT extract(EPOCH FROM now())").valid();
    auto const fld = res[0][0];

    // Nope!
    try {
        fld.as<std::chrono::system_clock::time_point>();
    } catch (Error const& err) {
    }

    // Ok.
    std::cout << fld.as<double>() << std::endl;
}
```
Finally you can read absolutely anything into `std::string`.
This doesn't perform any checks and just gives you raw content of the field.
There is also an option to avoid copying data with help of `std::string_view`,
but make sure the result is staying alive for enough time.
```
void resultData(Connection& conn) {
    auto const res = conn.exec("SELECT 'DATA'").valid();
    auto const fld = res[0][0];

    // Copying...
    std::cout << fld.as<std::string>() << std::endl;

    // ...and non-copying variants.
    std::cout << fld.as<std::string_view>() << std::endl;
}
```

<a name="escaping"/>

### Escaping

Thanks to the `Command`, it should be extremely rare when you have to deal with escaping.
But just in case there are a couple of methods performing that kind of task.
```
void escape(Connection& conn) {
    // Literals.
    std::cout << conn.esc("E'SCAPE_ME") << std::endl;

    // Identifiers.
    std::cout << conn.escId("escape me") << std::endl;
}
```

<a name="asynchronous-interface"/>

### Asynchronous interface

Statement execution methods considered so far are synchronous,
meaning that the calling thread is blocked until a database gives back the result.
Now lets look at an asynchronous family of methods
allowing you to split the execution process into sending and receiving phases.
Don't confuse it with multithreaded mode of a connection pool which is covered later.
That's how it looks like:
```
void send(Connection& conn) {
    // Sending doesn't block.
    auto receiver = conn.send("SELECT 123::INT").valid();

    while (receiver.isBusy()) {
        // Do some other job here...
    }

    // But receiving does block unless the result is ready.
    auto const res = receiver.receive();
    std::cout << res[0][0].as<int>() << std::endl;

    // No more results.
    std::cout << receiver.receive().isDone() << std::endl;
}
```
Calling the `send()` method returns an instance of type `Receiver`
which allows to obtain the result later.
It is a RAII-type which performs some cleanup in its destructor
to leave the connection in a valid state ready for reuse.
As a consequence the destructor can block for a short period of time
until all the results are taken,
but it normally shouldn't be an issue assuming proper library use.

You can't have multiple active sends simultaneously.
Either receive the results until `isDone()` gives true
or let the receiver go out of scope.
```
void sendTWice(Connection& conn) {
    auto rec1 = conn.send("SELECT 1").valid();

    // Error!
    try {
        auto rec2 = conn.send("SELECT 2").valid();
    } catch (Error const& err) {
    }
}
```
There are also asynchronous counterparts for prepared and raw statements.
There is nothing special about them so we won't waste our time on examples.
What's more interesting is a so-called single row mode,
the primary goal of which it to receive large datasets.
Such a large that it is impossible or unreasonable to fit them in memory.
You may think of it as establishing a stream of rows.
As always there is a tradeoff - single row mode works a bit slower.
Lets look at an example:
```
void sendRowByRow(Connection& conn) {
    // Imagine this query to end up with billions of rows.
    auto const query = "SELECT 1::INT"
                       " UNION ALL SELECT 2::INT"
                       " UNION ALL SELECT 3::INT";

    // Receive the result one row at a time.
    for (auto const& res : conn.iter(query).valid()) {
        if (res.isEmpty()) {
            continue;
        }

        std::cout << res[0][0].as<int>() << std::endl;
    }
}
```
Notice that the result is checked for emptiness inside the loop body -
this is because of how libpq works, and you have always do the same thing.

<a name="generating-statements"/>

### Generating statements

Since the library was not intended to be a fully-fledged SQL-statement generator
it is capable of producing just the most basic statements for you.
It is possible to create and drop tables,
perform inserts, selects and updates having no extra clauses.

This feature may come in handy when testing or prototyping,
but real-world applications often require more sophisticated SQL-statements,
and you can find some helpers in the library to construct them.
Remember the very first code example?
We've created a table called "my_table" there
to demonstrate basic statement generation facilities.

Now lets use that table to show how we can perform an upsert.
The so-called "upsert" is a special kind of Postgres statement
consisting of an insert which on conflict fallbacks to an update.
The starting example left our table in the following state:

id | info |        create_time
---|------|---------------------------
1  | foo  | 2019-03-21 13:01:25.729536
2  | bar  | 2019-03-21 13:01:25.729536
3  | baz  | 2019-03-21 13:01:25.729536

Lets update it:
```
using postgres::Statement;
using postgres::RangeStatement;

void myTableUpdate(Connection& conn) {
    // Needed for the example to work.
    conn.exec("ALTER TABLE my_table ADD PRIMARY KEY (id)").check();

    auto const now = std::chrono::system_clock::now();

    // 2 and 3 collide with existing ids.
    std::vector<MyTable> data{{2, "spam", now},
                              {3, "ham",  now},
                              {4, "eggs", now}};

    auto const range = std::pair{data.begin(), data.end()};

    // Generate an upsert statement.
    auto const upsert = "INSERT INTO "
                        + Statement<MyTable>::table()
                        + " ("
                        + Statement<MyTable>::fields()
                        + ") VALUES "
                        + RangeStatement::placeholders(range.first, range.second)
                        + " ON CONFLICT (id) DO UPDATE SET info = EXCLUDED.info";

    conn.exec(Command{upsert, range}).check();
}
```
We've just changed the content to the following:

id | info |        create_time
---|------|----------------------------
1  | foo  | 2019-03-21 13:01:25.729536
2  | spam | 2019-03-21 13:46:04.580402
3  | ham  | 2019-03-21 13:46:04.580402
4  | eggs | 2019-03-21 13:46:04.693358

Recall the definition of MyTable:
```
struct MyTable {
    int                                   id;
    std::string                           info;
    std::chrono::system_clock::time_point create_time;

    POSTGRES_CXX_TABLE("my_table", id, info, create_time);
};
```
It is the `POSTGRES_CXX_TABLE` macro that does the magic.
Once we've added it to a type definition it becomes possible
to visit all the data members along with their names using generated methods.
Those methods are `visitPostgresDefinition()` and `visitPostgresFields()`,
and you can use them to produce SQL-statements for your custom data types.
Here is a code skeleton to start with:
```
struct Generator {
    // Called by visitPostgresDefinition.
    template <typename T>
    void accept(char const* column_name) {
        std::cout << column_name << std::endl;
    }

    // Called by visitPostgresFields.
    template <typename T>
    void accept(char const* column_name, T const& value) {
        std::cout << column_name << std::endl;
    }
};

void myTableVisit(Connection& conn) {
    Generator gen{};
    MyTable::visitPostgresDefinition(gen);

    MyTable data{1, "foo"};
    data.visitPostgresFields(gen);
}
```
You might be wondering, how did the library figure out the field types
when we asked it to create a table for the definition of `MyTable`?
The rules are summarized here:

PostgreSQL type  | C++ types (with possible example for 64-bit machine)
-----------------|------------------------------------------------------
BOOL             | bool
REAL             | Floating point numbers fitting in 4 bytes (float).
DOUBLE PRECISION | Floating point numbers fitting in 8 bytes (double).
SMALLINT         | Signed integral numbers fitting in 2 bytes (short).
INT              | Signed integral numbers fitting in 4 bytes (int).
BIGINT           | Signed integral numbers fitting in 8 bytes (long).
SMALLSERIAL      | Unsigned integral numbers fitting in 2 bytes (unsigned short).
SERIAL           | Unsigned integral numbers fitting in 4 bytes (unsigned int).
BIGSERIAL        | Unsigned integral numbers fitting in 8 bytes (unsigned long).
TEXT             | std::string
TIMESTAMP        | std::chrono::system_clock::time_point

Be careful working with unsigned integers since the SQL standard doesn't support them
and neither do Postgres.
Moreover it is considered to be a good practice in C++ to use signed numbers for arithmetic
and unsigned ones for bitmasks.
The design decision for table generation was to utilize unsigned integers
to create auto-incremented fields, which are useful for producing unique identifiers.

<a name="connection-pool"/>

### Connection pool

Now that you know how to use a connection lets move on to a higher-level feature.
Connection pool was designed to execute multiple statements concurrently.
You've seen it in the "Get started with connection pool" section.
Here we'll explore all the details and nuances.
To start using the pool create an instance of a `Client` class.
An interface is rather compact: you can invoke either `exec()` or `query()`,
passing any callable object accepting a reference to a connection as its parameter.
Which one to use depends on the connection method which is going to do the job.
```
void pool() {
    Client cl{};

    // The result is of type std::future<Result>.
    auto res = cl.query([](Connection& conn) {
        return conn.exec("SELECT 1");
    });

    std::cout << res.get().valid().size() << std::endl;
}
```
The `Client` implements single-producer-multiple-consumers pattern
and is not thread-safe by itself: protect it with a mutex for concurrent access.
The interface is quite straightforward to use,
however a lot of flexibility is hidden in a connection pool configuration,
so lets discover it.

First of all any available connection option can be passed
to a client to let it know how to establish a connection.
We've covered how to configure a connection in the corresponding section.
The only difference is that a config or URL must be wrapped in a `Context`
to be passed to a client.
```
using postgres::Context;

void poolConfig() {
    auto cfg = Config::Builder{}.user("cxx_client")
                                .password("cxx_client")
                                .dbname("cxx_client")
                                .build();

    Client cl{Context::Builder{}.config(std::move(cfg)).build()};
}
```
The same technique is used for prepared statements:
```
void poolPrepare() {
    Client cl{Context::Builder{}.prepare({"my_select", "SELECT 1"}).build()};
}
```
And finally there are parameters affecting the behaviour of a connection pool:
```
using postgres::ShutdownPolicy;

void poolBehaviour() {
    Client cl{Context::Builder{}.idleTimeout(1min)
                                .maxConcurrency(2)
                                .maxQueueSize(30)
                                .shutdownPolicy(ShutdownPolicy::DROP)
                                .build()};
}
```
Idle timeout causes a thread to stop and close its connection to a database
after specified duration of inactivity.
Its primary purpose is to reduce the number of allocated resources back to the usual level
after load spikes has gone.
This feature is disabled by default.

Maximum concurrency specifies the number of threads/connections
and defaults to hardware concurrency.
Also the internal queue size can be limited.
Exceeding the limit results in an exception in a thread calling the client methods.
By default the queue is allowed to grow until application run out of memory and crash.

Shutdown policy regulates how to handle the queue on shutdown.
Default policy is to stop gracefully: all requests waiting in the queue will be executed.
You can alternatively choose to drop the queue,
but active requests are not canceled and can take some time to complete anyway.
And the last one policy is to abort resulting in an undefined behaviour.

