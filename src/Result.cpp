#include <postgres/Result.h>

#include <postgres/Error.h>
#include <postgres/Tuple.h>

namespace postgres {

Result::Result(PGresult* const handle)
    : handle_{handle, PQclear} {
}

Result::Result(Result&& other) noexcept = default;

Result& Result::operator=(Result&& other) noexcept = default;

Result::~Result() noexcept = default;

Result Result::valid() {
    _POSTGRES_CXX_ASSERT(isOk(), message());
    return std::move(*this);
}

Result const& Result::valid() const {
    _POSTGRES_CXX_ASSERT(isOk(), message());
    return *this;
}

bool Result::isOk() const {
    switch (type()) {
        case PGRES_COMMAND_OK:
        case PGRES_TUPLES_OK:
        case PGRES_SINGLE_TUPLE:
        case PGRES_NONFATAL_ERROR: {
            return true;
        }
        default: {
            break;
        }
    }
    return false;
}

bool Result::isDone() const {
    return native() == nullptr;
}

bool Result::isEmpty() const {
    return size() == 0;
}

Result::iterator Result::begin() const {
    return iterator{*valid().handle_, 0};
}

Result::iterator Result::end() const {
    return iterator{*valid().handle_, size()};
}

Tuple Result::operator[](int const idx) const {
    return *iterator{*valid().handle_, idx};
}

int Result::size() const {
    return PQntuples(native());
}

int Result::effect() const {
    std::string const s = PQcmdTuples(native());
    return s.empty() ? 0 : std::stoi(s);
}

const char* Result::message() const {
    return PQresultErrorMessage(native());
}

const char* Result::describe() const {
    return PQresStatus(type());
}

ExecStatusType Result::type() const {
    return PQresultStatus(native());
}

PGresult* Result::native() const {
    return handle_.get();
}

Result::iterator::iterator(PGresult& handle, int const idx)
    : handle_{&handle}, idx_{idx} {
}

Result::iterator::iterator(iterator const& other) = default;

Result::iterator& Result::iterator::operator=(iterator const& other) = default;

Result::iterator::iterator(iterator&& other) noexcept = default;

Result::iterator& Result::iterator::operator=(iterator&& other) noexcept = default;

Result::iterator::~iterator() noexcept = default;

bool Result::iterator::operator==(iterator const& other) const {
    return (handle_ == other.handle_) && (idx_ == other.idx_);
}

bool Result::iterator::operator!=(iterator const& other) const {
    return !(*this == other);
}

void Result::iterator::operator++() {
    ++idx_;
}

Result::iterator const Result::iterator::operator++(int) {
    return Result::iterator{*handle_, idx_++};
}

Tuple Result::iterator::operator->() const {
    return this->operator*();
}

Tuple Result::iterator::operator*() const {
    auto const n = PQntuples(handle_);
    _POSTGRES_CXX_ASSERT(idx_ < n, "No tuple " << idx_ << " in the result of size " << n);
    return Tuple{*handle_, idx_};
}

}  // namespace postgres
