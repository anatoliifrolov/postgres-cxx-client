#pragma once

namespace postgres {

class Client;

class Transaction {
    friend class Client;

public:
    Transaction(const Transaction& other) = delete;
    Transaction(Transaction&& other);
    Transaction& operator=(const Transaction& other) = delete;
    Transaction& operator=(Transaction&& other);
    ~Transaction();

    void commit();

private:
    explicit Transaction(Client& client);

    Client* client_;
};

}  // namespace postgres
