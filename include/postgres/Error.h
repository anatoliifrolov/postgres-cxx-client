#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

namespace postgres {

class Error : public std::exception {
public:
    explicit Error(std::string msg);
    Error(Error const& other);
    Error& operator=(Error const& other);
    Error(Error&& other) noexcept;
    Error& operator=(Error&& other) noexcept;
    ~Error() noexcept override;

    char const* what() const noexcept override;

private:
    std::string msg_;
};

class LogicError : public Error {
public:
    explicit LogicError(std::string msg);
    LogicError(LogicError const& other);
    LogicError& operator=(LogicError const& other);
    LogicError(LogicError&& other) noexcept;
    LogicError& operator=(LogicError&& other) noexcept;
    ~LogicError() noexcept override;
};

class RuntimeError : public Error {
public:
    explicit RuntimeError(std::string msg);
    RuntimeError(RuntimeError const& other);
    RuntimeError& operator=(RuntimeError const& other);
    RuntimeError(RuntimeError&& other) noexcept;
    RuntimeError& operator=(RuntimeError&& other) noexcept;
    ~RuntimeError() noexcept override;
};

}  // namespace postgres

#define _POSTGRES_CXX_FAIL(T, msg) \
    { \
        std::stringstream stream{}; \
        stream << "PostgreSQL client error: " << msg; \
        throw postgres::T{stream.str()}; \
    }

#define _POSTGRES_CXX_ASSERT(T, cond, msg) \
    if (!(cond)) { \
        _POSTGRES_CXX_FAIL(T, msg); \
    }
