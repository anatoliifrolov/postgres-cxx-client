#include <postgres/Consumer.h>
#include <postgres/Status.h>

namespace postgres {

Consumer::Consumer(std::shared_ptr<PGconn> handle, int is_ok)
    : handle_{std::move(handle)}, is_ok_{is_ok == 1} {
}

Consumer::Consumer(Consumer&& other) noexcept = default;

Consumer& Consumer::operator=(Consumer&& other) noexcept = default;

Consumer::~Consumer() noexcept {
    while (!consume().isDone()) {
    }
}

Status Consumer::consume() {
    return Status{PQgetResult(handle_.get())};
}

bool Consumer::isOk() {
    return is_ok_;
}

bool Consumer::isBusy() {
    PQconsumeInput(handle_.get());
    return PQisBusy(handle_.get()) == 1;
}

}  // namespace postgres
