#pragma once

#include <postgres/Consumer.h>

namespace postgres {

class Client;
class Result;

class Receiver : public Consumer {
    friend class Client;

public:
    Receiver(Receiver const& other) = delete;
    Receiver& operator=(Receiver const& other) = delete;
    Receiver(Receiver&& other) noexcept;
    Receiver& operator=(Receiver&& other) noexcept;
    ~Receiver() noexcept;

    Receiver valid();
    Result receive();

private:
    explicit Receiver(std::shared_ptr<PGconn> handle, int is_ok);

    bool iter();
};

}  // namespace postgres
