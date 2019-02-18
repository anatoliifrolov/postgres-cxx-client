#include <postgres/Receiver.h>

#include <postgres/Error.h>
#include <postgres/Result.h>

namespace postgres {

Receiver::Receiver(std::shared_ptr<PGconn> handle, int const res)
    : handle_{std::move(handle)}, is_ok_{res == 1} {
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

//Receiver Receiver::execRowByRow(Command const& cmd) {
//    return execAsync(cmd) && (PQsetSingleRowMode(handle_.get()) == 1);
//}
//
//Receiver Receiver::execPreparedRowByRow(Command const& cmd) {
//    return execPreparedAsync(cmd) && (PQsetSingleRowMode(handle_.get()) == 1);
//}

}  // namespace postgres
