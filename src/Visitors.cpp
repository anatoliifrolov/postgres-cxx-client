#include <postgres/internal/Visitors.h>

namespace postgres {
namespace internal {

void FieldsCollector::accept(char const* const name) {
    if (!res.empty()) {
        res += ",";
    }
    res += name;
};

void PlaceholdersCollector::accept(char const* const name) {
    res += res.empty() ? "$" : ",$";
    res += std::to_string(++idx);
};

void AssignmentsCollector::accept(char const* const name) {
    if (!res.empty()) {
        res += ",";
    }
    res += name;
    res += "=$";
    res += std::to_string(++idx);
};

}  // namespace internal
}  // namespace postgres
