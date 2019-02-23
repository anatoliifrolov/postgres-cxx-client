#pragma once

namespace postgres {

class Connection;
class Result;

class Transaction {
public:
    Transaction(Transaction const& other) = delete;
    Transaction& operator=(Transaction const& other) = delete;
    Transaction(Transaction&& other) noexcept;
    Transaction& operator=(Transaction&& other) noexcept;
    ~Transaction() noexcept;

    void commit();
    Result complete(Result res);

private:
    friend class Connection;

    explicit Transaction(Connection& conn);

    Connection* conn_;
};

}  // namespace postgres
