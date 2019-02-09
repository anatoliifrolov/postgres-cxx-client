#pragma once

#include <postgres/Tuple.h>
#include <postgres/Status.h>

namespace postgres {

class Result : public Status {
public:
    class iterator;

    explicit Result(PGresult* const handle);
    Result(const Result& other) = delete;
    Result(Result&& other);
    Result& operator=(const Result& other) = delete;
    Result& operator=(Result&& other);
    ~Result();

    // Data access methods.
    // Must be called only if isOk() returns true.
    iterator begin() const;
    iterator end() const;
    Tuple front() const;
    Tuple operator[](const int index) const;
};

class Result::iterator {
    friend class Result;

public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = Tuple;
    using pointer = Tuple*;
    using reference = Tuple&;

    iterator(const iterator& other);
    iterator(iterator&& other);
    iterator& operator=(const iterator& other);
    iterator& operator=(iterator&& other);
    ~iterator();

    bool operator==(const iterator& other) const;
    bool operator!=(const iterator& other) const;
    void operator++();
    iterator operator++(int);
    Tuple operator*() const;
    Tuple operator->() const;

private:
    explicit iterator(PGresult& handle, const int row_index);
    int size() const;

    PGresult* handle_;
    int row_index_;
};

}  // namespace postgres
