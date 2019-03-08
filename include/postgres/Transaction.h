#pragma once

#include <postgres/Result.h>

namespace postgres {

class Connection;

class Transaction {
public:
    Transaction(Transaction const& other) = delete;
    Transaction& operator=(Transaction const& other) = delete;
    Transaction(Transaction&& other) noexcept;
    Transaction& operator=(Transaction&& other) noexcept;
    ~Transaction() noexcept;

    Result commit();
    Transaction valid()&&;
    Status const& status() const;

private:
    friend class Connection;

    explicit Transaction(Connection& conn, Result status);

    Connection* conn_;
    Result status_;
};

}  // namespace postgres
