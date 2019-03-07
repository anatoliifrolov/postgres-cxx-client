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

Result Result::valid()&& {
    check();
    return std::move(*this);
}

Result::iterator Result::begin() const {
    check();
    return iterator{*native(), 0};
}

Result::iterator Result::end() const {
    check();
    return iterator{*native(), size()};
}

Row Result::operator[](int const idx) const {
    check();
    return *iterator{*native(), idx};
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
    _POSTGRES_CXX_ASSERT(LogicError,
                         (0 <= idx_) && (idx_ < PQntuples(handle_)),
                         "row index " << idx_ << " is out of range");
    return Row{*handle_, idx_};
}

}  // namespace postgres
