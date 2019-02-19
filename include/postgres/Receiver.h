#pragma once

#include <memory>
#include <libpq-fe.h>

namespace postgres {

class Result;

class Receiver {
    friend class Client;

public:
    Receiver(Receiver const& other) = delete;
    Receiver& operator=(Receiver const& other) = delete;
    Receiver(Receiver&& other) noexcept;
    Receiver& operator=(Receiver&& other) noexcept;
    ~Receiver() noexcept;

    Receiver valid();
    Result receive();
    bool isOk();
    bool isBusy();

private:
    explicit Receiver(std::shared_ptr<PGconn> handle, int is_ok);

    bool setRowByRow();

    std::shared_ptr<PGconn> handle_;
    bool                    is_ok_ = false;
};

}  // namespace postgres
