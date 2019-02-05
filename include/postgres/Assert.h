#pragma once

#include <stdexcept>
#include <sstream>

#define POSTGRES_CXX_FAIL(msg) \
    { \
        std::stringstream stream{}; \
        stream << msg; \
        throw std::runtime_error{stream.str()}; \
    }

#define POSTGRES_CXX_ASSERT(cond, msg) \
    if (!(cond)) { \
        POSTGRES_CXX_FAIL(msg); \
    }
