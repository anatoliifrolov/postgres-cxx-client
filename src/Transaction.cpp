#include <postgres/Transaction.h>
#include <postgres/Client.h>
#include <postgres/Error.h>

namespace postgres {

Transaction::Transaction(Client& client)
    : client_{&client} {
    client.execute("BEGIN");
}

Transaction::Transaction(Transaction&& other) {
    client_ = other.client_;
    other.client_ = nullptr;
}

Transaction& Transaction::operator=(Transaction&& other) {
    if (this != &other) {
        client_ = other.client_;
        other.client_ = nullptr;
    }
    return *this;
}

Transaction::~Transaction() {
    if (client_) {
        client_->execute("ROLLBACK");
    }
}

void Transaction::commit() {
    _POSTGRES_CXX_ASSERT(client_, "Transaction expired");
    client_->execute("COMMIT");
    client_ = nullptr;
}

}  // namespace postgres
