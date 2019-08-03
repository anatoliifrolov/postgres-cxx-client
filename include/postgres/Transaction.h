#pragma once

namespace postgres {

class Connection;
class Result;

class Transaction {
public:
    Transaction(Transaction const& other) = delete;
    Transaction& operator=(Transaction const& other) = delete;
    Transaction(Transaction&& other) noexcept;
    Transaction& operator=(Transaction&& other) = delete;
    ~Transaction() noexcept;

    Result commit();

private:
    friend class Connection;

    explicit Transaction(Connection& conn);

    Connection* conn_;
};

}  // namespace postgres
