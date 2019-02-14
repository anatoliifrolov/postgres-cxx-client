#pragma once

#include <chrono>
#include <ctime>
#include <string>
#include <type_traits>
#include <optional>
#include <libpq-fe.h>
#include <postgres/internal/Assert.h>
#include <postgres/Oid.h>
#include <postgres/internal/Bytes.h>
#include <postgres/internal/Classifier.h>

namespace postgres {

class Field {
    friend class Tuple;

public:
    Field(const Field& other);
    Field(Field&& other);
    Field& operator=(const Field& other);
    Field& operator=(Field&& other);
    ~Field();

    bool isNull() const;
    const char* const name() const;

    template <typename Dst>
    operator Dst() const {
        return as<Dst>();
    }

    template <typename Dst>
    Dst as() const {
        Dst tmp{};
        *this >> tmp;
        return tmp;
    }

    template <typename Dst>
    void operator>>(std::optional<Dst>& dst) const {
        if (isNull()) {
            return;
        }
        dst = as<Dst>();
    }

    template <typename Dst>
    std::enable_if_t<internal::isPlain<Dst>()> operator>>(Dst& dst) const {
        auto ptr = &dst;
        *this >> ptr;
        _POSTGRES_CXX_ASSERT(ptr,
                             "Cannot store NULL value of field " << name() << " into reference");
    }

    template <typename Dst>
    std::enable_if_t<internal::isPlain<Dst>()> operator>>(Dst*& dst) const {
        if (isNull()) {
            dst = nullptr;
            return;
        }
        read(*dst);
    }

private:
    explicit Field(PGresult& result, const int row_index, const int column_index);

    template <typename Dst>
    std::enable_if_t<std::is_arithmetic<Dst>::value> read(Dst& dst) const {
        auto constexpr size = static_cast<int>(sizeof(Dst));
        auto const     len  = PQgetlength(result_, row_index_, column_index_);
        auto const     type = PQftype(result_, column_index_);

        _POSTGRES_CXX_ASSERT(isBinary(),
                             "Cannot store text field "
                                 << name()
                                 << " into object of arithmetic type");
        _POSTGRES_CXX_ASSERT(len <= size,
                             "Cannot store field "
                                 << name()
                                 << " which is of size "
                                 << len
                                 << " bytes into object of size "
                                 << size
                                 << " bytes");

        switch (type) {
            case BOOLOID:
            case CHAROID: {
                dst = *value();
                break;
            }
            case INT2OID: {
                dst = castBinary<Dst, int16_t>();
                break;
            }
            case INT4OID: {
                dst = castBinary<Dst, int32_t>();
                break;
            }
            case INT8OID: {
                dst = castBinary<Dst, int64_t>();
                break;
            }
            case FLOAT4OID: {
                static_assert(sizeof(float) == 4, "Unexpected float size");
                dst = castBinary<Dst, float>();
                break;
            }
            case FLOAT8OID: {
                static_assert(sizeof(double) == 8, "Unexpected double size");
                dst = castBinary<Dst, double>();
                break;
            }
            default: {
                _POSTGRES_CXX_FAIL("Column " << name() << " is of unexpected type " << type);
            }
        }
    }

    template <typename Dst, typename Src>
    Dst castBinary() const {
        return static_cast<Dst>(internal::orderBytes<Src>(value()));
    };

    void read(bool& dst) const;
    void read(std::string& dst) const;
    void read(time_t& dst) const;
    void read(std::chrono::system_clock::time_point& dst) const;
    bool isBinary() const;
    const char* value() const;

    PGresult* result_;
    int row_index_;
    int column_index_;
};

}  // namespace postgres
