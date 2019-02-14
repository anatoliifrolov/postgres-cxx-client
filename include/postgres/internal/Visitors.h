#pragma once

#include <string>

namespace postgres {
namespace internal {

struct FieldsCollector {
    void accept(char const* name);

    std::string res;
};

struct PlaceholdersCollector {
    void accept(char const* name);

    std::string res;
    int         idx = 0;
};

struct AssignmentsCollector {
    void accept(char const* name);

    std::string res;
    int         idx = 0;
};

}  // namespace internal
}  // namespace postgres
