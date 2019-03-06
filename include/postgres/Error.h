#pragma once

#include <sstream>
#include <stdexcept>

namespace postgres {

struct Error : std::runtime_error {
    explicit Error(std::string const& msg);
};

}  // namespace postgres

#define _POSTGRES_CXX_FAIL(msg) \
    { \
        std::stringstream stream{}; \
        stream << "postgres client error: " << msg; \
        throw postgres::Error{stream.str()}; \
    }

#define _POSTGRES_CXX_ASSERT(cond, msg) \
    if (!(cond)) { \
        _POSTGRES_CXX_FAIL(msg); \
    }
