#include <postgres/Transaction.h>

#include <postgres/Connection.h>
#include <postgres/Error.h>

namespace postgres {

Transaction::Transaction(Connection& conn)
    : conn_{&conn} {
}

Transaction::Transaction(Transaction&& other) noexcept
    : conn_{other.conn_} {
    other.conn_ = nullptr;
}

Transaction::~Transaction() noexcept {
    if (conn_) {
        conn_->exec("ROLLBACK");
    }
}

Result Transaction::commit() {
    _POSTGRES_CXX_ASSERT(LogicError, conn_, "no transaction in progress");
    auto const conn = conn_;
    conn_ = nullptr;
    return conn->exec("COMMIT");
}

}  // namespace postgres
