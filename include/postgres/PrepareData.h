#pragma once

#include <string>
#include <vector>
#include <libpq-fe.h>

namespace postgres {

struct PrepareData {
    std::string name_;
    std::string body_;
    std::vector<Oid> types_;
};

}  // namespace postgres
