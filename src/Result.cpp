#include <postgres/Result.h>
#include <postgres/Error.h>

namespace postgres {

Result::Result(PGresult* const handle)
    : Status{handle} {
}

Result::Result(Result&& other) = default;

Result& Result::operator=(Result&& other) = default;

Result::~Result() = default;

Result::iterator Result::begin() const {
    validate();
    return iterator{*handle_, 0};
}

Result::iterator Result::end() const {
    validate();
    return iterator{*handle_, size()};
}

Tuple Result::front() const {
    return *begin();
}

Tuple Result::operator[](const int index) const {
    validate();
    return *iterator{*handle_, index};
}

Result::iterator::iterator(PGresult& handle, const int row_index)
    : handle_{&handle}, row_index_{row_index} {
}

Result::iterator::iterator(const iterator& other) = default;

Result::iterator::iterator(iterator&& other) = default;

Result::iterator& Result::iterator::operator=(const iterator& other) = default;

Result::iterator& Result::iterator::operator=(iterator&& other) = default;

Result::iterator::~iterator() = default;

bool Result::iterator::operator==(const iterator& other) const {
    return handle_ == other.handle_ && row_index_ == other.row_index_;
}

bool Result::iterator::operator!=(const iterator& other) const {
    return !(*this == other);
}

void Result::iterator::operator++() {
    ++row_index_;
}

Result::iterator Result::iterator::operator++(int) {
    return Result::iterator{*handle_, row_index_++};
}

Tuple Result::iterator::operator*() const {
    _POSTGRES_CXX_ASSERT(row_index_ < size(),
                         "No tuple " << row_index_ << " in the result of size " << size());
    return Tuple{*handle_, row_index_};
}

Tuple Result::iterator::operator->() const {
    return this->operator*();
}

int Result::iterator::size() const {
    return PQntuples(handle_);
}

}  // namespace postgres
