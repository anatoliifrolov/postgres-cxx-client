#pragma once

#include <stdexcept>
#include <sstream>

#define _POSTGRES_CXX_FAIL(msg) \
    { \
        std::stringstream stream{}; \
        stream << msg; \
        throw std::runtime_error{stream.str()}; \
    }

#define _POSTGRES_CXX_ASSERT(cond, msg) \
    if (!(cond)) { \
        _POSTGRES_CXX_FAIL(msg); \
    }
