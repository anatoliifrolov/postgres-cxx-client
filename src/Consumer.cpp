#include <postgres/Consumer.h>

#include <postgres/Error.h>
#include <postgres/Status.h>

namespace postgres {

Consumer::Consumer(std::shared_ptr<PGconn> handle, int const is_ok)
    : handle_{std::move(handle)}, is_ok_{is_ok == 1} {
    _POSTGRES_CXX_ASSERT(RuntimeError,
                         isOk(),
                         "fail to send statement: " << PQerrorMessage(handle_.get()));
}

Consumer::Consumer(Consumer&& other) noexcept = default;

Consumer& Consumer::operator=(Consumer&& other) noexcept = default;

Consumer::~Consumer() noexcept {
    while (!consume().isDone()) {
    }
}

Status Consumer::consume() {
    return Status{PQgetResult(handle_.get()), this};
}

bool Consumer::isOk() const {
    return is_ok_;
}

bool Consumer::isBusy() {
    PQconsumeInput(handle_.get());
    return PQisBusy(handle_.get()) == 1;
}

}  // namespace postgres
