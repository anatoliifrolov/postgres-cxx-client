#include <postgres/Receiver.h>

#include <postgres/Error.h>
#include <postgres/Result.h>

namespace postgres {

Receiver::Receiver(std::shared_ptr<PGconn> handle, int const is_ok)
    : handle_{std::move(handle)}, is_ok_{is_ok == 1} {
}

Receiver::Receiver(Receiver&& other) noexcept = default;

Receiver& Receiver::operator=(Receiver&& other) noexcept = default;

Receiver::~Receiver() noexcept {
    while (!receive().isDone()) {
    }
}

Receiver Receiver::valid() {
    _POSTGRES_CXX_ASSERT(isOk(), PQerrorMessage(handle_.get()));
    return std::forward<Receiver>(*this);
}

Result Receiver::receive() {
    return Result{PQgetResult(handle_.get())};
}

bool Receiver::isOk() {
    return is_ok_;
}

bool Receiver::isBusy() {
    PQconsumeInput(handle_.get());
    return PQisBusy(handle_.get()) == 1;
}

bool Receiver::setRowByRow() {
    is_ok_ = is_ok_ && (PQsetSingleRowMode(handle_.get()) == 1);
    return is_ok_;
}

}  // namespace postgres
