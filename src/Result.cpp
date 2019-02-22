#include <postgres/Result.h>

#include <postgres/Error.h>
#include <postgres/Row.h>

namespace postgres {

Result::Result(PGresult* const handle)
    : Status{handle} {
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

Result::iterator Result::begin() const {
    return iterator{*valid().native(), 0};
}

Result::iterator Result::end() const {
    return iterator{*valid().native(), size()};
}

Row Result::operator[](int const idx) const {
    return *iterator{*valid().native(), idx};
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

Row Result::iterator::operator->() const {
    return this->operator*();
}

Row Result::iterator::operator*() const {
    auto const n = PQntuples(handle_);
    _POSTGRES_CXX_ASSERT(idx_ < n, "No tuple " << idx_ << " in the result of size " << n);
    return Row{*handle_, idx_};
}

}  // namespace postgres
