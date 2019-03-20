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
