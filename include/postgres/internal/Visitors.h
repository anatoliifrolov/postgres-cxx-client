#pragma once

#include <string>

namespace postgres {
namespace internal {

template <typename T>
struct FieldsCollector {
    void accept(char const* const name) {
        if (!res_.empty()) {
            res_ += ",";
        }
        res_ += name;
    };

    std::string res_;
};

template <typename T>
struct PlaceholdersCollector {
    void accept(char const* const name) {
        res_ += res_.empty() ? "$" : ",$";
        res_ += std::to_string(++idx_);
    };

    std::string res_;
    int         idx_ = 0;
};

template <typename T>
struct AssignmentsCollector {
    void accept(char const* const name) {
        if (!res_.empty()) {
            res_ += ",";
        }
        res_ += name;
        res_ += "=$";
        res_ += std::to_string(++idx_);
    };

    std::string res_;
    int         idx_ = 0;
};

}  // namespace internal
}  // namespace postgres
