#pragma once

#include <string>
#include <vector>
#include <libpq-fe.h>

namespace postgres {

struct PreparingStatement {
    std::string      name;
    std::string      body;
    std::vector<Oid> types;
};

}  // namespace postgres
