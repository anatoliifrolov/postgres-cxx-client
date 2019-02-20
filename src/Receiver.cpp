#include <postgres/Receiver.h>
#include <postgres/Error.h>

namespace postgres {

Receiver::Receiver(std::shared_ptr<PGconn> handle, int const is_ok)
    : Consumer{std::move(handle), is_ok} {
}

Receiver::Receiver(Receiver&& other) noexcept = default;

Receiver& Receiver::operator=(Receiver&& other) noexcept = default;

Receiver::~Receiver() noexcept = default;

Receiver Receiver::valid() {
    _POSTGRES_CXX_ASSERT(isOk(), PQerrorMessage(handle_.get()));
    return std::move(*this);
}

Result Receiver::receive() {
    return Result{PQgetResult(handle_.get())};
}

void Receiver::iter() {
    is_ok_ = is_ok_ && (PQsetSingleRowMode(handle_.get()) == 1);
}

Receiver::iterator Receiver::begin() {
    return iterator{*this, receive()};
}

Receiver::iterator Receiver::end() {
    return iterator{*this, Result{nullptr}};
}

Receiver::iterator::iterator(Receiver& rec, Result res)
    : rec_{&rec}, res_{std::move(res)}, is_{!res_.isDone()} {
}

Receiver::iterator::iterator(iterator&& other) noexcept = default;

Receiver::iterator& Receiver::iterator::operator=(iterator&& other) noexcept = default;

Receiver::iterator::~iterator() noexcept = default;

bool Receiver::iterator::operator==(iterator const& other) const {
    return (rec_ == other.rec_) && (is_ == other.is_);
}

bool Receiver::iterator::operator!=(iterator const& other) const {
    return !(*this == other);
}

void Receiver::iterator::operator++() {
    *this = rec_->begin();
}

Receiver::iterator const Receiver::iterator::operator++(int) {
    return rec_->begin();
}

Result Receiver::iterator::operator->() {
    return this->operator*();
}

Result Receiver::iterator::operator*() {
    return std::move(res_);
}

}  // namespace postgres
