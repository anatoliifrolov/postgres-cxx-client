#pragma once

namespace postgres {

class Client;
class Connection;
class Result;

class Transaction {
    friend class Client;
    friend class Connection;

public:
    Transaction(Transaction const& other) = delete;
    Transaction& operator=(Transaction const& other) = delete;
    Transaction(Transaction&& other) noexcept;
    Transaction& operator=(Transaction&& other) noexcept;
    ~Transaction() noexcept;

    void commit();
    Result complete(Result res);

private:
    explicit Transaction(Connection& cl);

    Connection* cl_;
};

}  // namespace postgres
