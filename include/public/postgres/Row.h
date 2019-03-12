#pragma once

#include <string>
#include <type_traits>
#include <libpq-fe.h>
#include <postgres/internal/Classifier.h>
#include <postgres/Field.h>

namespace postgres {

class Row {
public:
    Row(Row const& other);
    Row& operator=(Row const& other);
    Row(Row&& other) noexcept;
    Row& operator=(Row&& other) noexcept;
    ~Row() noexcept;

    template <typename T>
    std::enable_if_t<internal::isVisitable<T>(), Row&> operator>>(T& val) {
        val.visitPostgresFields(*this);
        return *this;
    };

    template <typename T>
    std::enable_if_t<!internal::isVisitable<T>(), Row&> operator>>(T& val) {
        (*this)[col_idx_++] >> val;
        return *this;
    }

    template <typename T>
    void accept(char const* const name, T& val) {
        (*this)[name] >> val;
    };

    Field operator[](std::string const& col_name) const;
    Field operator[](char const* col_name) const;
    Field operator[](int col_idx) const;

    int size() const;

private:
    friend class Result;

    explicit Row(PGresult& res, int row_idx);

    PGresult* res_;
    int row_idx_;
    int col_idx_;
};

}  // namespace postgres
