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
