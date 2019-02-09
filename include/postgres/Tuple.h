#pragma once

#include <string>
#include <libpq-fe.h>
#include <postgres/Field.h>
#include <postgres/Classifier.h>

namespace postgres {

class Tuple {
    friend class Result;

public:
    Tuple(const Tuple& other);
    Tuple(Tuple&& other);
    Tuple& operator=(const Tuple& other);
    Tuple& operator=(Tuple&& other);
    ~Tuple();

    int size() const;
    Field operator[](const std::string& column_name) const;
    Field operator[](const char* const column_name) const;
    Field operator[](const int column_index) const;

    template <typename T>
    Tuple& operator>>(T& val) {
        read(val);
        return *this;
    }

    template <typename T>
    void accept(const char* const table, const char* const name, T& val) {
        (*this)[name] >> val;
    };

private:
    explicit Tuple(PGresult& result, const int row_index);
    void validateIndex(const int column_index) const;

    template <typename T>
    std::enable_if_t<internal::isVisitable<T>()>
    read(T& val) {
        val.visitPostgresFields(*this);
    };

    template <typename T>
    std::enable_if_t<!internal::isVisitable<T>()>
    read(T& val) {
        validateIndex(column_index_);
        (*this)[column_index_++] >> val;
    }

    PGresult* result_;
    int row_index_;
    int column_index_;
};

}  // namespace postgres
