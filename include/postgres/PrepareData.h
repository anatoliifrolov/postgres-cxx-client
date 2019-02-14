#pragma once

#include <string>
#include <vector>
#include <libpq-fe.h>

namespace postgres {

struct PrepareData {
    std::string      name;
    std::string      body;
    std::vector<Oid> types;
};

}  // namespace postgres
