#pragma once

#include <memory>
#include <libpq-fe.h>

namespace postgres {

class Status {
public:
    explicit Status(PGresult* handle);
    Status(Status const& other) = delete;
    Status& operator=(Status const& other) = delete;
    Status(Status&& other) noexcept;
    Status& operator=(Status&& other) noexcept;
    ~Status() noexcept;

    Status const& valid() const;
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
    std::unique_ptr<PGresult, void (*)(PGresult*)> handle_;
};

}  // namespace postgres
