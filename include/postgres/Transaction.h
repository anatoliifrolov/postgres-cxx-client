#pragma once

namespace postgres {

class Client;
class Result;

class Transaction {
    friend class Client;

public:
    Transaction(Transaction const& other) = delete;
    Transaction& operator=(Transaction const& other) = delete;
    Transaction(Transaction&& other) noexcept;
    Transaction& operator=(Transaction&& other) noexcept;
    ~Transaction() noexcept;

    void commit();
    Result complete(Result res);

private:
    explicit Transaction(Client& cl);

    Client* cl_;
};

}  // namespace postgres
