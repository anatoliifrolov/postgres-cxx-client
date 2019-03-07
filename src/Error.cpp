#include <postgres/Error.h>

namespace postgres {

Error::Error(std::string msg)
    : msg_{std::move(msg)} {
}

Error::Error(Error const& other) = default;

Error& Error::operator=(Error const& other) = default;

Error::Error(Error&& other) noexcept = default;

Error& Error::operator=(Error&& other) noexcept = default;

Error::~Error() noexcept = default;

char const* Error::what() const noexcept {
    return msg_.data();
}

LogicError::LogicError(std::string msg)
    : Error{std::move(msg)} {
}

LogicError::LogicError(LogicError const& other) = default;

LogicError& LogicError::operator=(LogicError const& other) = default;

LogicError::LogicError(LogicError&& other) noexcept = default;

LogicError& LogicError::operator=(LogicError&& other) noexcept = default;

LogicError::~LogicError() noexcept = default;

RuntimeError::RuntimeError(std::string msg)
    : Error{std::move(msg)} {
}

RuntimeError::RuntimeError(RuntimeError const& other) = default;

RuntimeError& RuntimeError::operator=(RuntimeError const& other) = default;

RuntimeError::RuntimeError(RuntimeError&& other) noexcept = default;

RuntimeError& RuntimeError::operator=(RuntimeError&& other) noexcept = default;

RuntimeError::~RuntimeError() noexcept = default;

}  // namespace postgres
