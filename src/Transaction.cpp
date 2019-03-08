#include <postgres/Transaction.h>

#include <utility>
#include <postgres/Connection.h>

namespace postgres {

Transaction::Transaction(Connection& conn, Result status)
    : conn_{&conn}, status_{std::move(status)} {
}

Transaction::Transaction(Transaction&& other) noexcept = default;

Transaction& Transaction::operator=(Transaction&& other) noexcept = default;

Transaction::~Transaction() noexcept {
    if (status_.isOk()) {
        conn_->exec("ROLLBACK");
    }
}

Result Transaction::commit() {
    status_.check();
    status_ = conn_->exec("COMMIT");
    return std::move(status_);
}

Transaction Transaction::valid()&& {
    status_.check();
    return std::move(*this);
}

Status const& Transaction::status() const {
    return status_;
}

}  // namespace postgres
