#include <utility>
#include <postgres/internal/Assert.h>
#include <postgres/Tuple.h>

namespace postgres {

Tuple::Tuple(PGresult& result, const int row_index)
    : result_{&result}, row_index_{row_index}, column_index_{0}
{}

Tuple::Tuple(const Tuple& other) = default;

Tuple::Tuple(Tuple&& other) = default;

Tuple& Tuple::operator=(const Tuple& other) = default;

Tuple& Tuple::operator=(Tuple&& other) = default;

Tuple::~Tuple() = default;

int Tuple::size() const {
    return PQnfields(result_);
}

Field Tuple::operator[](const std::string& column_name) const {
    return (*this)[column_name.c_str()];
}

Field Tuple::operator[](const char* const column_name) const {
    const auto column_index = PQfnumber(result_, column_name);
    _POSTGRES_CXX_ASSERT(0 <= column_index, "No column " << column_name);
    return Field{*result_, row_index_, column_index};
}

Field Tuple::operator[](const int column_index) const {
    validateIndex(column_index);
    return Field{*result_, row_index_, column_index};
}

void Tuple::validateIndex(const int column_index) const {
    _POSTGRES_CXX_ASSERT(
        0 <= column_index && column_index < size(),
        "No column " << column_index << " in the tuple of size " << size());
}

}  // namespace postgres
