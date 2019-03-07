#pragma once

#include <memory>
#include <libpq-fe.h>

namespace postgres {

class Status {
public:
    Status(Status const& other) = delete;
    Status& operator=(Status const& other) = delete;
    Status(Status&& other) noexcept;
    Status& operator=(Status&& other) noexcept;
    ~Status() noexcept;

    void check() const;
    bool isOk() const;
    bool isDone() const;
    bool isEmpty() const;

    int size() const;
    int effect() const;
    const char* message() const;
    const char* describe() const;
    ExecStatusType type() const;

    PGresult* native() const;

protected:
    friend class Connection;
    friend class Consumer;

    explicit Status(PGresult* handle);

private:
    std::unique_ptr<PGresult, void (*)(PGresult*)> handle_;
};

}  // namespace postgres
