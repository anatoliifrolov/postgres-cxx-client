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
