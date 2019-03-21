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

void config() {
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
void configStr() {
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

void args(Connection& conn) {
    conn.exec(Command{"SELECT $1, $2", 42, "foo"}).check();
}
/// ```
/// Argument types are automatically detected by the ```Command```,
/// but sometimes you have to be explicit.
/// In the example below if we hadn't specified the type of the argument
/// it would've been guessed to be text instead of json:
/// ```
using postgres::bindOid;

void argsOid(Connection& conn) {
    conn.exec(Command{"SELECT $1", bindOid(R"({"foo": "bar"})", JSONOID)}).check();
}
/// ```
/// If there are arguments possibly having NULL values,
/// use pointers of ```std::std::optional``` type.
/// In the following example both ```ptr``` and ```opt``` will be treated as NULLs:
/// ```
void argsNull(Connection& conn) {
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
void argsLarge(Connection& conn) {
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
void argsDyna(Connection& conn) {
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

void argsTime(Connection& conn) {
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
void execMultiBad(Connection& conn) {
    try {
        conn.exec("SELECT 1; SELECT 2").check();
    } catch (Error const& err) {
    }
}
/// ```
/// But this can be useful when applying a migration.
/// That's why the ```Connection``` provides a special method:
/// ```
void execMultiOk(Connection& conn) {
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
/// ```
/// You can cast the field to arithmetic type, but the rules are quite strict.
/// In particular the following is prohibited:
/// - loss of precision (casting from floating point value to integral one and vice versa);
/// - narrowing (casting larger type to smaller);
/// - underflow (reading negative values into variables of unsigned types).
///
/// Lets look how those three cases may appear in code:
/// ```
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
/// ```
/// Also the library is able to read timestamps without time zones:
/// ```
void resultTime(Connection& conn) {
    auto const res = conn.exec("SELECT '2017-08-25T13:03:35'::TIMESTAMP").valid();
    auto const fld = res[0][0];

    // Modern C++ way.
    fld.as<std::chrono::system_clock::time_point>();

    // Getting time_t.
    fld.as<Time>().toUnix();
}
/// ```
/// But what if time zone information is crutial to your app?
/// Trying to treat this as in the previous example will lead to runtime error.
/// But you can just cast timestamp to ```TEXT``` and read it into ```std::string```:
/// ```
void resultTimeZone(Connection& conn) {
    auto const res = conn.exec("SELECT now()::TEXT").valid();
    auto const fld = res[0][0];

    // Prints something like '2019-03-21 12:58:13.256812+03'.
    std::cout << fld.as<std::string>() << std::endl;
}
/// ```
/// And one more caveat about working with timestamps.
/// Using this library eliminates the need for extracting epoch from timestamp
/// by means of Postgres, so avoid this in a new code.
/// But in case you have to deal with such a statement beware that
/// it yields the result of type double precision:
/// ```
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
/// ```
/// Finally you can read absolutely anything into ```std::string```.
/// This doesn't perform any checks and just gives you raw content of the field.
/// There is also an option to avoid copying data using ```std::string_view```
/// but make sure the result is staying alive for enough time.
/// ```
void resultData(Connection& conn) {
    auto const res = conn.exec("SELECT 'DATA'").valid();
    auto const fld = res[0][0];

    // Copying...
    std::cout << fld.as<std::string>() << std::endl;

    // ...and non-copying variants.
    std::cout << fld.as<std::string_view>() << std::endl;
}
/// ```

/// ### Escaping
///
/// Thanks to ```Command``` class it should be extremely rare you have to deal with escaping.
/// But just in case there are a couple of methods performing that kind of task.
/// ```
void escape(Connection& conn) {
    // Literals.
    std::cout << conn.esc("E'SCAPE_ME") << std::endl;

    // Identifiers.
    std::cout << conn.escId("escape me") << std::endl;
}
/// ```

/// ### Asynchronous interface
///
/// Statement execution methods considered so far are synchronous,
/// meaning that the calling thread is blocked until a database gives back the result.
/// There is also an asynchronous family of methods
/// allowing you to split the execution process into sending and receiving parts.
/// Don't confuse it with multithreaded mode of connection pool which is covered later.
/// That's how it looks like:
/// ```
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
/// ```
/// Calling the ```send``` method returns an instance of type ```Receiver```
/// which allows you to obtain the result later.
/// It is a RAII-type which performs cleanup in its destructor
/// leaving the connection in a valid state ready for reuse.
/// As a consequence the destructor can block for a short time until all the results are taken,
/// but it normally shouldn't be an issue assuming proper library use.
///
/// You can't have multiple active sends simultaneously.
/// Either receive the results until ```isDone``` gives true
/// or just let the receiver go out of scope.
/// ```
void sendTWice(Connection& conn) {
    auto rec1 = conn.send("SELECT 1").valid();

    // Error!
    try {
        auto rec2 = conn.send("SELECT 2").valid();
    } catch (Error const& err) {
    }
}
/// ```
/// There are also asynchronous counterparts for prepared and raw statements.
/// There is nothing special about them so we won't waste our time on examples.
/// What's more interesting is a so called single row mode.
/// The primary goal of this mode it to receive a large datasets.
/// Such a large that it is impossible or unreasonable to fit them in memory.
/// You may think of it as establishing a stream of rows.
/// As always there is a tradeoff - the single row mode works a bit slower.
/// Lets look at an example:
/// ```
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
/// ```
/// Notice that the result is checked to be not empty inside the loop body -
/// you have always do the same thing.
/// This is because of how libpq works.

/// ### Statement generation
///
/// As the library was not intended to be a fully-fledged SQL-statement generator
/// it is capable of producing just the most basic statements for you.
/// It is possible to create and drop tables,
/// perform inserts, selects and updates having no extra clauses.
///
/// This feature might come in handy when testing or prototyping,
/// but for real-world applications we most likely to end up with more complex SQL-statement.
/// And the library can help us here a little bit.
/// Remember the very first code example?
/// We've created a table called "my_table" there
/// to demonstrate basic statement generation facilities.
///
/// Now lets use that table to show how we can make an "upsert" statement.
/// The so-called "upsert" is a special kind of Postgres statement
/// consisting of an insert followed with update when insert results in a conflict.
/// The starting example left our table in the following state:
///
/// id | info |        create_time
/// ---|------|---------------------------
/// 1  | foo  | 2019-03-21 13:01:25.729536
/// 2  | bar  | 2019-03-21 13:01:25.729536
/// 3  | baz  | 2019-03-21 13:01:25.729536
///
/// Lets update it:
/// ```
void myTableUpdate(Connection& conn) {
    // Needed for the example to work.
    conn.exec("ALTER TABLE my_table ADD PRIMARY KEY (id)").check();

    auto const now = std::chrono::system_clock::now();

    // 3 and 4 collide with existing ids.
    std::vector<MyTable> data{{2, "spam", now},
                              {3, "ham",  now},
                              {4, "eggs", now}};

    auto const range = std::pair{data.begin(), data.end()};

    // Generate an upsert statement.
    using postgres::Statement;
    using postgres::RangeStatement;

    auto const statement = "INSERT INTO "
                           + Statement<MyTable>::table()
                           + " VALUES "
                           + RangeStatement::placeholders(range.first, range.second)
                           + " ON CONFLICT (id) DO UPDATE SET info = EXCLUDED.info";

    conn.exec(Command{statement, range}).check();
}
/// ```
/// As a result the table now contains:
///
/// id | info |        create_time
/// ---|------|----------------------------
/// 1  | foo  | 2019-03-21 13:01:25.729536
/// 2  | spam | 2019-03-21 13:46:04.580402
/// 3  | ham  | 2019-03-21 13:46:04.580402
/// 4  | eggs | 2019-03-21 13:46:04.693358
///
/// Recall the definition of MyTable:
/// ```
/// struct MyTable {
///     int                                   id;
///     std::string                           info;
///     std::chrono::system_clock::time_point create_time;
///
///     POSTGRES_CXX_TABLE("my_table", id, info, create_time);
/// };
/// ```
/// It is ```POSTGRES_CXX_TABLE``` macro that does all the magic.
/// Once we've added it to a structure definition it becomes possible
/// to visit all the data members along with their names using generated methods.
/// Those methods are ```visitPostgresDefinition``` and ```visitPostgresFields```,
/// and you can use them to produce SQL-statements for your custom data types.
/// Here is a code skeleton to start with:
/// ```
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
/// ```

/// ### Connection pool
///
/// Now that you know how to use the connection lets move on to more high-level feature.
/// Connection pool is designed to execute statements concurrently in separate threads.
/// You've seen it in the section called "Get started with connection pool".
/// Here we'll explore all the details and nuances.
/// To start using the connection pool you have to create an instance of a ```Client``` class.
/// An interface is rather compact: you can invoke either ```exec``` or ```query``` methods,
/// passing any callable object accepting a reference to a connection as its parameter.
/// Which one to use depends on the connection method which is going to do the job.
/// ```
void pool() {
    Client cl{};

    auto res = cl.query([](Connection& conn) {
        return conn.exec("SELECT 1");
    });

    std::cout << res.get().valid().size() << std::endl;
}
/// ```
/// The client implements single-producer-multiple-consumers pattern
/// and is not thread-safe by itself
/// meaning you have to protect it with a mutex to use from within multiple threads.
/// The client is quite straightforward to use,
/// however a lot of flexibility is hidden in its configuration,
/// so lets discover how to configure the client.
///
/// First of all any available connection option can be passed
/// to the client to let it know how to establish a connection.
/// We've covered how to configure a connection in the corresponding section.
/// The only difference is that a config or URL must be wrapped in a ```Context```
/// to be passed to a client.
/// ```
using postgres::Context;

void poolConfig() {
    auto cfg = Config::Builder{}.user("cxx_client")
                                .password("cxx_client")
                                .dbname("cxx_client")
                                .build();

    Client cl{Context::Builder{}.config(std::move(cfg)).build()};
}
/// ```
/// The same technique is used for prepared statements:
/// ```
void poolPrepare() {
    Client cl{Context::Builder{}.prepare({"my_select", "SELECT 1"}).build()};
}
/// ```
/// And finally there are parameters affecting the behaviour of the connection pool:
/// ```
using postgres::ShutdownPolicy;

void poolBehaviour() {
    Client cl{Context::Builder{}.idleTimeout(1min)
                                .maxConcurrency(2)
                                .maxQueueSize(30)
                                .shutdownPolicy(ShutdownPolicy::DROP)
                                .build()};
}
/// ```
/// Idle timeout tells the client to stop thread and close connection to a database
/// after specified duration of inactivity.
/// Its primary purpose is to reduce the number of allocated resources after load spikes
/// back to the usual level.
/// This feature is disabled by default.
///
/// Maximum concurrency specifies the number of threads/connections
/// and defaults to hardware concurrency.
/// Also the internal queue size can be limited.
/// Exceeding the limit results in an exception in a thread working with the client.
/// By default the queue is allowed to grow until application run out of memory and crash.
///
/// Shutdown policy regulates how to handle the queue on shutdown.
/// Default policy is to stop gracefully
/// meaning that all the requests waiting in queue will be executed.
/// You can alternatively choose to drop the queue,
/// but active requests are not canceled and can take some time anyway.
/// And the last one policy is to abort.
/// Behaviour in that case is undefined and your app have good chances to crash.
