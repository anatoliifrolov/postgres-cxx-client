#pragma once

#include <memory>
#include <libpq-fe.h>
#include <postgres/Fwd.h>

namespace postgres {

class Receiver {
    friend class Connection;

public:
    Receiver(Receiver const& other);
    Receiver& operator=(Receiver const& other);
    Receiver(Receiver&& other) noexcept;
    Receiver& operator=(Receiver&& other) noexcept;
    ~Receiver() noexcept;

    Receiver valid();
    Result receive();
    bool isOk();
    bool isBusy();

private:
    explicit Receiver(std::shared_ptr<PGconn> handle, int res);

    std::shared_ptr<PGconn> handle_;
    bool                    is_ok_ = false;
};

}  // namespace postgres
