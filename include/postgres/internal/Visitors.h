#pragma once

#include <chrono>
#include <string>
#include <type_traits>

namespace postgres::internal {

struct FieldsCollector {
    template <typename T>
    void accept(char const* const name) {
        if (!res.empty()) {
            res += ",";
        }
        res += name;
    }

    std::string res;
};

struct TypedFieldsCollector {
    template <typename T>
    void accept(char const* const name) {
        if (!res.empty()) {
            res += ",";
        }
        res += name;
        res += " ";
        res += type(static_cast<T*>(nullptr));
    }

    std::string res;

private:
    template <typename T>
    std::enable_if_t<std::is_arithmetic_v<T>, char const*> type(T*) {
        if (std::is_same_v<T, bool>) {
            return "BOOL";
        }

        auto constexpr SIZE = sizeof(T);
        if (std::is_floating_point_v<T>) {
            if (SIZE <= 4) {
                return "REAL";
            }
            return "DOUBLE PRECISION";
        }

        if (std::is_signed_v<T>) {
            if (SIZE <= 2) {
                return "SMALLINT";
            }
            if (SIZE <= 4) {
                return "INT";
            }
            return "BIGINT";
        }

        if (SIZE <= 2) {
            return "SMALLSERIAL";
        }
        if (SIZE <= 4) {
            return "SERIAL";
        }
        return "BIGSERIAL";
    }

    char const* type(std::string*) {
        return "TEXT";
    }

    char const* type(std::chrono::system_clock::time_point*) {
        return "TIMESTAMP";
    }
};

struct PlaceholdersCollector {
    template <typename T>
    void accept(char const* const name) {
        res += res.empty() ? "$" : ",$";
        res += std::to_string(++idx);
    }

    int         idx = 0;
    std::string res;
};

struct AssignmentsCollector {
    template <typename T>
    void accept(char const* const name) {
        if (!res.empty()) {
            res += ",";
        }
        res += name;
        res += "=$";
        res += std::to_string(++idx);
    }

    int         idx = 0;
    std::string res;
};

}  // namespace postgres::internal
