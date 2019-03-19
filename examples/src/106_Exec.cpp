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
/// Argument types are automatically recognized by the ```Command```,
/// but sometimes you have to be explicit.
/// In the example below if we hadn't specified the type of the argument
/// it would've been guessed to be text instead of json:
/// ```
using postgres::bindOid;

void execOid(Connection& conn) {
    conn.exec(Command{"SELECT $1", bindOid(R"({"foo": "bar"})", JSONOID)}).check();
}
/// ```
