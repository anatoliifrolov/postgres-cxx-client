#include <postgres/Transaction.h>

#include <postgres/Connection.h>
#include <postgres/Error.h>

namespace postgres {

Transaction::Transaction(Connection& conn)
    : conn_{&conn} {
    conn.exec("BEGIN");
}

Transaction::Transaction(Transaction&& other) noexcept {
    conn_ = other.conn_;
    other.conn_ = nullptr;
}

Transaction& Transaction::operator=(Transaction&& other) noexcept {
    if (this != &other) {
        conn_ = other.conn_;
        other.conn_ = nullptr;
    }
    return *this;
}

Transaction::~Transaction() noexcept {
    if (conn_) {
        conn_->exec("ROLLBACK");
    }
}

void Transaction::commit() {
    _POSTGRES_CXX_ASSERT(conn_, "Transaction expired");
    conn_->exec("COMMIT");
    conn_ = nullptr;
}

Result Transaction::complete(Result res) {
    auto const conn = conn_;
    conn_ = nullptr;
    if (!res.isOk()) {
        if (conn->isOk()) {
            conn->exec("ROLLBACK");
        }
        return res;
    }

    auto commit_res = conn->exec("COMMIT");
    if (!commit_res.isOk()) {
        return commit_res;
    }

    return res;
}

}  // namespace postgres
