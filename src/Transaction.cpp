#include <postgres/Transaction.h>

#include <postgres/Client.h>
#include <postgres/Error.h>

namespace postgres {

Transaction::Transaction(Client& cl)
    : cl_{&cl} {
    cl.exec("BEGIN");
}

Transaction::Transaction(Transaction&& other) noexcept {
    cl_ = other.cl_;
    other.cl_ = nullptr;
}

Transaction& Transaction::operator=(Transaction&& other) noexcept {
    if (this != &other) {
        cl_ = other.cl_;
        other.cl_ = nullptr;
    }
    return *this;
}

Transaction::~Transaction() noexcept {
    if (cl_) {
        cl_->exec("ROLLBACK");
    }
}

void Transaction::commit() {
    _POSTGRES_CXX_ASSERT(cl_, "Transaction expired");
    cl_->exec("COMMIT");
    cl_ = nullptr;
}

Result Transaction::complete(Result res) {
    auto const cl = cl_;
    cl_ = nullptr;
    if (!res.isOk()) {
        if (cl->isOk()) {
            cl->exec("ROLLBACK");
        }
        return res;
    }

    auto commit_res = cl->exec("COMMIT");
    if (!commit_res.isOk()) {
        return commit_res;
    }

    return res;
}

}  // namespace postgres
