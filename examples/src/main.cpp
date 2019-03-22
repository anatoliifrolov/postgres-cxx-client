#include <postgres/Connection.h>

using postgres::Connection;

void getStarted();
void getStartedPool();

void config();
void configStr();
void configUrl();
void configBuilder();
void configBuilderExtra();
void configBuilderManual();

void connect();
void connectCheck();
void connectReset(Connection& conn);

void exec(Connection& conn);
void execNoexcept(Connection& conn);
void args(Connection& conn);
void argsOid(Connection& conn);
void argsNull(Connection& conn);
void argsLarge(Connection& conn);
void argsRange(Connection& conn);
void argsAfter(Connection& conn);
void argsTime(Connection& conn);

void prepare(Connection& conn);

void execMultiBad(Connection& conn);
void execMultiOk(Connection& conn);

void transact(Connection& conn);
void transactManual(Connection& conn);

void result(Connection& conn);
void resultVars(Connection& conn);
void resultNull(Connection& conn);
void resultBadCast(Connection& conn);
void resultTime(Connection& conn);
void resultTimeZone(Connection& conn);
void resultExtractEpoch(Connection& conn);
void resultData(Connection& conn);

void escape(Connection& conn);

void send(Connection& conn);
void sendTWice(Connection& conn);
void sendRowByRow(Connection& conn);

void myTableUpdate(Connection& conn);
void myTableVisit(Connection& conn);

void pool();
void poolConfig();
void poolPrepare();
void poolBehaviour();

int main() {
    Connection conn{};
    conn.exec("DROP TABLE IF EXISTS my_table").check();

    getStarted();
    getStartedPool();

    config();
    configStr();
    configUrl();
    configBuilder();
    configBuilderExtra();
    configBuilderManual();

    connect();
    connectCheck();
    connectReset(conn);

    exec(conn);
    execNoexcept(conn);
    args(conn);
    argsOid(conn);
    argsNull(conn);
    argsLarge(conn);
    argsRange(conn);
    argsAfter(conn);
    argsTime(conn);

    prepare(conn);

    execMultiBad(conn);
    execMultiOk(conn);

    transact(conn);
    transactManual(conn);

    result(conn);
    resultVars(conn);
    resultNull(conn);
    resultBadCast(conn);
    resultTime(conn);
    resultTimeZone(conn);
    resultExtractEpoch(conn);
    resultData(conn);

    escape(conn);

    send(conn);
    sendTWice(conn);
    sendRowByRow(conn);

    myTableUpdate(conn);
    myTableVisit(conn);

    pool();
    poolConfig();
    poolPrepare();
    poolBehaviour();
}
