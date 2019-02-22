#pragma once

#include <string>
#include <type_traits>
#include <libpq-fe.h>
#include <postgres/Field.h>
#include <postgres/internal/Classifier.h>

namespace postgres {

class Row {
public:
    Row(Row const& other);
    Row& operator=(Row const& other);
    Row(Row&& other) noexcept;
    Row& operator=(Row&& other) noexcept;
    ~Row() noexcept;

    template <typename T>
    Row& operator>>(T& val) {
        read(val);
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

    template <typename T>
    std::enable_if_t<internal::isVisitable<T>()> read(T& val) {
        val.visitPostgresFields(*this);
    };

    template <typename T>
    std::enable_if_t<!internal::isVisitable<T>()> read(T& val) {
        validate(col_idx_);
        (*this)[col_idx_++] >> val;
    }

    void validate(int col_idx) const;

    PGresult* res_;
    int row_idx_;
    int col_idx_;
};

}  // namespace postgres
