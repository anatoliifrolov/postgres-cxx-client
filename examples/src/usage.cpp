/// ## Usage

/// ### Get started with connection
///
/// The following example gives you a basic idea of how to use the library.
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
/// ```

/// ### Get started with connection pool
///
/// Here is an example demonstrating how to use a connection pool.
/// ```
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

/// ### What to include
///
/// The library provides all-in-one header file ```#include <postgres/Postgres.h>```,
/// one with forward declarations ```#include <postgres/Fwd.h>```,
/// and also each class that is a part of a public API has its own header.
/// It is strongly discouraged to declare any of the library types in your project code.
/// So include ```Fwd.h``` when you just need a declaration, say in a function signature.
/// If compilation time is not a big concern use ```Postgres.h```.
/// Otherwise include only needed files from ```postgres``` directory.

/// ### Configuring
///
/// Beign a wrapper around libpq, the library just gives you a convinient interface to its features.
/// So this documentation mostly desribes the interface and is not a comprehensive reference.
/// For further details see official libpq docs https://www.postgresql.org/docs/11/libpq-connect.html.
///
/// Postgres has default values for every of its configuration parameter.
/// For instance, username defaults to the operating system name of the user running the app,
/// and database name is the same as the user name.
/// You have several ways to override the defaults:
/// - environment variables;
/// - connect string;
/// - URL;
/// - configuration builder.
///
/// The following four examples essentially do the same thing:
/// ```
#include <postgres/Postgres.h>

using postgres::Config;
using postgres::Connection;

void configDefault() {
    Connection conn{};
    conn.check();
}
/// ```
/// Here we connect to a database using the default values and environment variables.
/// This is a good choise to pass sensitive information like passwords.
/// For example, the library is tested assuming that PGUSER, PGPASSWORD and PGDATABASE are set.
///
/// Another way is to use a connect string:
/// ```
void configConnectStr() {
    Connection conn{"user=cxx_client password=cxx_client dbname=cxx_client"};
    conn.check();
}
/// ```
/// ...or URL:
/// ```
void configUrl() {
    Connection conn{"postgresql://cxx_client:cxx_client@/cxx_client"};
    conn.check();
}
/// ```
/// And the last one option is to use a configuration builder:
/// ```
void configBuilder() {
    Connection conn{Config::Builder{}.user("cxx_client")
                                     .password("cxx_client")
                                     .dbname("cxx_client")
                                     .build()};
    conn.check();
}
/// ```
/// The class ```Config::Builder``` provides setter methods for all parameters
/// available (and not deprecated) at the moment of writing.
/// Method names are in *snake_case* to exactly mirror corresponding parameter names.
/// An example setting some random parameters:
/// ```
#include <chrono>

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
/// ```
/// Also there are some general purpose setters.
/// The following example manually sets the same parameters as the previous one:
/// ```
void configBuilderManual() {
    Connection conn{Config::Builder{}.set("application_name", "APP")
                                     .enable("keepalives", true)
                                     .setNumber("keepalives_count", 2)
                                     .setInterval("keepalives_idle", 3min)
                                     .set("sslmode", "disable")
                                     .build()};
    conn.check();
}
/// ```

/// ### Error handling
///
/// One of the library design goals was to eliminate some sorts of bugs by design and at compile time.
/// But of course runtime errors are unavoidable.
/// Most of the time you're given two ways to deal with them depending on your attitude to exceptions.
/// Lets talk about exceptions first.
///
/// An exception classes hierarchy consists of a base class ```Error```
/// and two classes derived from it: ```LogicError``` and ```RuntimeError```
/// inside namespace ```postgres```.
/// The ```Error``` in turn is derived from ```std::exception```.
/// The library throws ```LogicError``` to indicate a bug in your code,
/// such as trying to access a row that is out of bounds or misusing the library in some way.
/// Runtime errors, e.g. loosing a connection to a database,
/// don't lead to an exception unless you explicitly tell to do that.
///
/// Many library types provide two methods to check their status: ```isOk()```
/// returning boolean value and ```check()``` throwing an exception.
/// It is a common pattern used throughout the library, so remember it.
/// Also some types provide a method called ```valid()``` which can throw an exception as well.
/// This one is for convinience and only works in chained method calls.
/// You will encounter it in later examples.
///
/// Lets demonstrate the described behaviour using ```Connection``` class
/// which we're now should be familiar with:
/// ```
#include <iostream>
#include <postgres/Postgres.h>

using postgres::Connection;

void connect() {
    Connection conn{};
    if (conn.isOk()) {
        std::cout << "Connected to a database" << std::endl;
    } else {
        std::cerr << "Fail to connect to a database: " << conn.message() << std::endl;
    }
}
/// ```
/// And an example of how to work with exceptions:
/// ```
using postgres::Error;

void connectCheck() {
    try {
        Connection conn{};
        conn.check();
    } catch (Error const& err) {
        std::cerr << err.what() << std::endl;
    }
}
/// ```

/// ### Statement execution
///
/// No that we've learned how to connect to a database lets execute some SQL-statements.
/// That's what ```Connection::exec()``` method is for:
/// ```
#include <postgres/Postgres.h>

using postgres::Connection;

void exec(Connection& conn) {
    auto res = conn.exec("SELECT 1").valid();
    // Handle the result...
}
/// ```
/// The ```exec``` returns an object of type ```Result```
/// which is completely detached from the connection produced it,
/// meaning you can use it even after the connection has been destroyed.
///
/// Next we check that the result is ok calling the ```valid()``` method,
/// which throws an exception if the result is in a bad state,
/// and just returns it otherwise.
/// This is similar to calling ```unwrap()```, if you are familiar with Rust.
/// Alternatively you can check the result using ```isOk()``` method
/// as described in section about error handling.
///
/// Often it is needed to customize a statement with some arguments.
/// You have an option to embed argument values directly into the statement,
/// but it is a bad choise for several reasons:
/// - this could make your app vulnerable to SQL-injection attacks;
/// - you have to deal with escaping;
/// - data is passed as text instead of binary format.
///
/// The library provides a better solution:
/// ```
using postgres::Command;

void execArgs(Connection& conn) {
    conn.exec(Command{"SELECT $1, $2", 42, "foo"}).check();
}
/// ```
/// Argument types are automatically detected by the ```Command```,
/// but sometimes you have to be explicit.
/// In the example below if we hadn't specified the type of the argument
/// it would've been guessed to be text instead of json:
/// ```
using postgres::bindOid;

void oidArgs(Connection& conn) {
    conn.exec(Command{"SELECT $1", bindOid(R"({"foo": "bar"})", JSONOID)}).check();
}
/// ```
/// If there are arguments possibly having NULL values,
/// use pointers of ```std::std::optional``` type.
/// In the following example both ```ptr``` and ```opt``` will be treated as NULLs:
/// ```
void nullArgs(Connection& conn) {
    int* ptr = nullptr;
    std::optional<int> opt{};
    conn.exec(Command{"SELECT $1, $2", ptr, opt}).check();
}
/// ```
/// The ```Command``` stores its arguments into internal buffer.
/// Sometimes you might be willing to avoid copying, e.g. for large piece of text.
/// This can be achieved by passing pointer to underlying C-style string
/// or by using ```std::string_view``` type.
/// The same is true for statements as well.
/// The both way are shown below:
/// ```
void largeArgs(Connection& conn) {
    std::string      text = "SOME VERY LONG TEXT...";
    std::string_view view = text;
    conn.exec(Command{"SELECT $1, $2", text.data(), view}).check();
}
/// ```
/// That's how you can pass arguments stored in a container:
/// ```
void argsRange(Connection& conn) {
    std::vector<int> args{1, 2, 3};
    conn.exec(Command{"SELECT $1, $2, $3", std::pair{args.begin(), args.end()}}).check();
}
/// ```
/// Actually you're not limited to pass all the arguments at once to ```Command```.
/// There is an ability to add arguments dynamically:
/// ```
void dynaArgs(Connection& conn) {
    Command cmd{"SELECT $1, $2"};
    cmd << 42 << "foo";
    conn.exec(cmd).check();
}
/// ```
/// And a final note about timestamps.
/// The recommended way is to use a database type called ```TIMESTAMP```,
/// which represents a number of microseconds since Postgres epoch in UTC.
/// C++ ```std::chrono::system_clock::time_point``` objects are mapped to that type
/// and passed in binary form as integral numbers.
///
/// If you want to preserve time zone there is a special library type.
/// But keep in mind that it is a bit less efficient
/// because timestamps are converted to strings to be transmitted to Postgres:
/// ```
using postgres::Time;

void timeArgs(Connection& conn) {
    auto now = std::chrono::system_clock::now();
    conn.exec(Command{"SELECT $1", Time{now, true}}).check();
}
/// ```

/// ### Prepared statements
///
/// Using prepared statements is quite trivial.
/// The first step is to prepare statement.
/// You have to specify statement name, statement body and argument types if present.
/// Then you can use the name to actually execute the statement and bind argument values.
/// Consider an example:
/// ```
#include <postgres/Postgres.h>

using postgres::Connection;
using postgres::PreparedCommand;
using postgres::PrepareData;

void prepare(Connection& conn) {
    conn.exec(PrepareData{"my_select", "SELECT $1", {INT4OID}}).check();
    conn.exec(PreparedCommand{"my_select", 123}).check();
}
/// ```
/// Beware that statement is prepared on a connection.
/// There is a method to reset lost connection but it doesn't reprepare statements,
/// you have to do it manually each time after loosing a connection.

/// ### Multiple statements in one
///
/// The ```exec``` method described above allows to execute at most one statement at a time,
/// meaning that the following is a runtime error:
/// ```
void badMultiExec(Connection& conn) {
    try {
        conn.exec("SELECT 1; SELECT 2").check();
    } catch (Error const& err) {
    }
}
/// ```
/// But this can be useful when applying a migration.
/// That's why the ```Connection``` provides a special method:
/// ```
void goodMultiExec(Connection& conn) {
    conn.execRaw("SELECT 1; SELECT 2").check();
}
/// ```
/// That's not an error anymore, but there are a couple of limitations.
/// The first one is that there is no way to pass arguments, just a statement text.
/// And also you are not allowed to obtain data.
/// Don't be confused by the example - it is quite silly and just to demonstrate the feature,
/// normally there won't be selects.
/// The second limitation is due to the library sends and receives arguments in binary format,
/// but when multiple statements are passed there is no way to tell Postgres to enable binary mode.
/// Also when a select statement is embedded somewhere in between the other statements,
/// it is impossible to get the selected data
/// because only the result of the last statement is returned from a database.
/// Such an interface discourages you from trying to do this.
/// Once again, this mode is primarily to apply a migration.

/// ### Transactions
///
/// Each statement is executed as a separate transaction.
/// Multiple statements separated with semicolons as described in the previous section
/// are a single transaction as well.
/// If you need to join multiple statements into a single transaction, you have two options.
/// Lets start with the simplest one:
/// ```
void transact(Connection& conn) {
    conn.transact("SELECT 1", "SELECT 2", "SELECT 3").check();
}
/// ```
/// That's it. Just call the ```transact``` method on a connection instance.
/// Either all statements passed succeed or none of them have any effect.
/// Again the example is a bit ridiculous, but imagine statements to be more meaningful,
/// for instance inserting data to two different tables when one insert without the other
/// would leave a system in inconsistent state.
///
/// The second way gives you more fine-grained control over transaction execution:
/// ```
void transactManual(Connection& conn) {
    auto tx = conn.begin().valid();
    conn.exec("SELECT 1").check();
    conn.exec("SELECT 2").check();
    tx.commit().check();
}
/// ```
/// This way allows to put some logic between statement execution
/// and build transactions which have to be more complex and flexible.
/// Just don't forget to check that transaction started successfully and to commit it.
/// Also consider the possibility of commit operation itself to fail.
/// When the transaction handler returned from the call to ```begin()``` goes out of scope
/// it rollbacks the transaction unless it has been explicitly commited before.

/// ### Reading the result
///
/// Now it's time to talk about queries and how to access their results.
/// As mentioned above the ```exec``` method returns an object of type ```Result```.
/// Iterating over it will produce a ```Row``` object at each iteration.
/// The '''Row''' in turn consists of a number of ```Field```*s*
/// which can be obtained by index or name.
/// Finally you can read the value of the ```Field``` into a new variable or an existing one.
/// Probably that's a bit more clear expressed in code:
/// ```
void result(Connection& conn) {
    for (auto const& row : conn.exec("SELECT 'foo' AS foo, 'bar' AS bar").valid()) {
        std::cout
            << row["foo"].as<std::string>()
            << " "
            << row["bar"].as<std::string>()
            << std::endl;
    }
}
/// ```
/// Now lets store the same values into variables:
/// ```
void resultVars(Connection& conn) {
    std::string foo, bar;

    auto const res = conn.exec("SELECT 'foo' AS foo, 'bar' AS bar").valid();
    res[0]["foo"] >> foo;
    res[0]["bar"] >> bar;

    std::cout << foo << " " << bar << std::endl;
}
/// ```
/// In the last example it is evident that the result cannot be empty.
/// In practice you always have to check it before indexing the result to avoid errors.
/// Iterating lacks the possibility to access rows which are out of bounds
/// and therefore is safer and preferrable.
///
/// Similarly to ```Command``` NULLs are represented with pointers or ```std::optional```:
/// ```
void resultNull(Connection& conn) {
    auto const res = conn.exec("SELECT NULL::TEXT").valid();

    // Bad idea.
    std::string s;
    try {
        res[0][0] >> s;
    } catch (Error const& err) {
    }

    // Ok.
    auto opt = res[0][0].as<std::optional<std::string>>();

    // Also ok.
    auto ptr = &s;
    res[0][0] >> ptr;
}
/// ```
/// You can cast the field to arithmetic type, but the rules are quite strict.
/// In particular the following is prohibited:
/// - loosing precision (casting from floating point value to integral one and vice versa);
/// - narrowing (casting larger type to smaller);
/// - reading negative values into variables of unsigned types.
