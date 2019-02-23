#pragma once

#include <string>
#include <vector>
#include <postgres/Config.h>
#include <postgres/PreparingStatement.h>

namespace postgres {
namespace internal {

struct Context {
    Config                          cfg;
    std::string                     uri;
    std::vector<PreparingStatement> prep;
    int                             max_pool_size;
    int                             max_queue_size;
};

}  // namespace internal
}  // namespace postgres
