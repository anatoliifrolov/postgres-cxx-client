#pragma once

#include <memory>
#include <libpq-fe.h>

namespace postgres {

class Status {
public:
    explicit Status(PGresult* const handle);
    Status(const Status& other) = delete;
    Status(Status&& other);
    Status& operator=(const Status& other) = delete;
    Status& operator=(Status&& other);
    ~Status();

    // Status.
    bool isOk() const;
    bool isDone() const;
    void validate() const;
    operator bool() const;
    ExecStatusType status() const;
    const char* statusName() const;
    const char* errorMessage() const;

    // Result size info.
    // Safe to be called in any status.
    int size() const;
    int affected() const;
    bool empty() const;

    PGresult* native() const;

protected:
    std::unique_ptr<PGresult, void (*)(PGresult*)> handle_;
};

}  // namespace postgres
