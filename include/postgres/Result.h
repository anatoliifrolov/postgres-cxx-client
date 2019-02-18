#pragma once

#include <memory>
#include <libpq-fe.h>

namespace postgres {

class Tuple;

class Result {
public:
    class iterator;

    explicit Result(PGresult* handle);
    Result(Result const& other) = delete;
    Result& operator=(Result const& other) = delete;
    Result(Result&& other) noexcept;
    Result& operator=(Result&& other) noexcept;
    ~Result() noexcept;

    Result const& valid() const;
    bool isOk() const;
    bool isDone() const;
    bool isEmpty() const;

    iterator begin() const;
    iterator end() const;
    Tuple operator[](int idx) const;

    int size() const;
    int effect() const;
    const char* message() const;
    const char* describe() const;
    ExecStatusType type() const;

    PGresult* native() const;

private:
    std::unique_ptr<PGresult, void (*)(PGresult*)> handle_;
};

class Result::iterator {
    friend class Result;

public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = Tuple;
    using pointer = Tuple*;
    using reference = Tuple&;

    iterator(iterator const& other);
    iterator& operator=(iterator const& other);
    iterator(iterator&& other) noexcept;
    iterator& operator=(iterator&& other) noexcept;
    ~iterator() noexcept;

    bool operator==(iterator const& other) const;
    bool operator!=(iterator const& other) const;
    void operator++();
    iterator const operator++(int);
    Tuple operator->() const;
    Tuple operator*() const;

private:
    explicit iterator(PGresult& handle, int idx);

    PGresult* handle_ = nullptr;
    int idx_ = 0;
};

}  // namespace postgres
