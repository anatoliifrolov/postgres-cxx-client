#include <postgres/Connection.h>

int main() {
    return postgres::Connection::ping();
}
