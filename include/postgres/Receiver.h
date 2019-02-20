#pragma once

#include <postgres/Consumer.h>
#include <postgres/Result.h>

namespace postgres {

class Receiver : public Consumer {
public:
    class iterator;

    Receiver(Receiver const& other) = delete;
    Receiver& operator=(Receiver const& other) = delete;
    Receiver(Receiver&& other) noexcept;
    Receiver& operator=(Receiver&& other) noexcept;
    ~Receiver() noexcept;

    Receiver valid();
    Result receive();
    iterator begin();
    iterator end();

private:
    friend class Client;

    explicit Receiver(std::shared_ptr<PGconn> handle, int is_ok);

    void iter();
};

class Receiver::iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = Result;
    using pointer = Result*;
    using reference = Result&;

    iterator(iterator const& other) = delete;
    iterator& operator=(iterator const& other) = delete;
    iterator(iterator&& other) noexcept;
    iterator& operator=(iterator&& other) noexcept;
    ~iterator() noexcept;

    bool operator==(iterator const& other) const;
    bool operator!=(iterator const& other) const;
    void operator++();
    iterator const operator++(int);
    Result operator->();
    Result operator*();

private:
    friend class Receiver;

    explicit iterator(Receiver& rec, Result res);

    Receiver* rec_;
    Result res_;
    bool   is_;
};

}  // namespace postgres
