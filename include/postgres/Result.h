#pragma once

#include <postgres/Status.h>

namespace postgres {

class Row;

class Result : public Status {
public:
    class iterator;

    explicit Result(PGresult* handle);
    Result(Result const& other) = delete;
    Result& operator=(Result const& other) = delete;
    Result(Result&& other) noexcept;
    Result& operator=(Result&& other) noexcept;
    ~Result() noexcept;

    Result valid();
    Result const& valid() const;

    iterator begin() const;
    iterator end() const;
    Row operator[](int idx) const;
};

class Result::iterator {
    friend class Result;

public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = Row;
    using pointer = Row*;
    using reference = Row&;

    iterator(iterator const& other);
    iterator& operator=(iterator const& other);
    iterator(iterator&& other) noexcept;
    iterator& operator=(iterator&& other) noexcept;
    ~iterator() noexcept;

    bool operator==(iterator const& other) const;
    bool operator!=(iterator const& other) const;
    void operator++();
    iterator const operator++(int);
    Row operator->() const;
    Row operator*() const;

private:
    explicit iterator(PGresult& handle, int idx);

    PGresult* handle_ = nullptr;
    int idx_ = 0;
};

}  // namespace postgres
