#include <postgres/Row.h>
#include <postgres/Error.h>

namespace postgres {

Row::Row(PGresult& res, int const row_idx)
    : res_{&res}, row_idx_{row_idx}, col_idx_{0} {
}

Row::Row(Row const& other) = default;

Row& Row::operator=(Row const& other) = default;

Row::Row(Row&& other) noexcept = default;

Row& Row::operator=(Row&& other) noexcept = default;

Row::~Row() noexcept = default;

Field Row::operator[](std::string const& col_name) const {
    return (*this)[col_name.c_str()];
}

Field Row::operator[](char const* const col_name) const {
    auto const col_idx = PQfnumber(res_, col_name);
    _POSTGRES_CXX_ASSERT(LogicError, 0 <= col_idx, "no column '" << col_name << "'");
    return Field{*res_, row_idx_, col_idx};
}

Field Row::operator[](int const col_idx) const {
    _POSTGRES_CXX_ASSERT(LogicError,
                         0 <= col_idx && col_idx < size(),
                         "column index " << col_idx << " in out of range");
    return Field{*res_, row_idx_, col_idx};
}

int Row::size() const {
    return PQnfields(res_);
}

}  // namespace postgres
