#include <postgres/Transaction.h>
#include <postgres/Client.h>
#include <postgres/Assert.h>

namespace postgres {

Transaction::Transaction(Client& client) : client_{&client} {
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
        client_->tryExecute("ROLLBACK");
    }
}

void Transaction::commit() {
    POSTGRES_CXX_ASSERT(client_, "Transaction expired");
    client_->execute("COMMIT");
    client_ = nullptr;
}

}  // namespace postgres
