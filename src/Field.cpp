#include <utility>
#include <cstring>
#include <postgres/internal/Assert.h>
#include <postgres/Field.h>
#include <postgres/Timestamp.h>

namespace postgres {

Field::Field(PGresult& result, const int row_index, const int column_index)
    : result_{&result}, row_index_{row_index}, column_index_{column_index}
{}

Field::Field(const Field& other) = default;

Field::Field(Field&& other) = default;

Field& Field::operator=(const Field& other) = default;

Field& Field::operator=(Field&& other) = default;

Field::~Field() = default;

void Field::read(bool& dst) const {
    if (isBinary()) {
        read<bool>(dst);
        return;
    }
    static const char* vals[] = {
        "1", "0",
        "t", "f",
        "y", "n",
        "true", "false",
        "yes", "no",
        "on", "off"};
    const auto src = value();
    for (const auto& val : vals) {
        if (std::strcmp(val, src) == 0) {
            dst = ((val - *vals) % 2 == 0);
            return;
        }
    }
    _POSTGRES_CXX_FAIL("Cannot treat '" << src << "' as boolean");
}

void Field::read(std::string& dst) const {
    dst.assign(value(), PQgetlength(result_, row_index_, column_index_));
}

void Field::read(time_t& dst) const {
    switch (PQftype(result_, column_index_)) {
        case TIMESTAMPOID:
        case TIMESTAMPTZOID: {
            auto src = std::chrono::system_clock::time_point{};
            read(src);
            dst = std::chrono::system_clock::to_time_t(src);
            break;
        }
        default: {
            read<time_t>(dst);
            break;
        }
    }
}

void Field::read(std::chrono::system_clock::time_point& dst) const {
    const auto type = PQftype(result_, column_index_);
    _POSTGRES_CXX_ASSERT(type == TIMESTAMPOID, "Unexpected column type " << type);
    dst = (isBinary() ?
        makeTimestamp(std::chrono::microseconds{internal::orderBytes<int64_t>(value())}, postgresEpoch()) :
        makeTimestamp(value())).timePoint();
}

bool Field::isNull() const {
    return PQgetisnull(result_, row_index_, column_index_);
}

const char* const Field::name() const {
    return PQfname(result_, column_index_);
}

bool Field::isBinary() const {
    const auto format = PQfformat(result_, column_index_);
    if (format == 0) {
        return false;
    }
    if (format == 1) {
        return true;
    }
    _POSTGRES_CXX_FAIL("Unexpected column data format " << format);
}

const char* Field::value() const {
    return PQgetvalue(result_, row_index_, column_index_);
}

}  // namespace postgres
