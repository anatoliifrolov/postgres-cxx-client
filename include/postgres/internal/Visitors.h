#pragma once

#include <string>
#include <postgres/internal/Classifier.h>

namespace postgres {
namespace internal {

template <typename T>
struct FieldsList {
    static const std::string& get() {
        static const auto cache = [] {
            FieldsList<T> visitor{};
            T::visitPostgresDefinition(visitor);
            return visitor.res_;
        }();
        return cache;
    }

    void accept(const char* const table, const char* const name) {
        if (!res_.empty()) {
            res_ += ",";
        }
        res_ += name;
    };

private:
    std::string res_;
};

template <typename T>
struct AssigmentsList {
    static const std::string& get() {
        static const auto cache = [] {
            AssigmentsList<T> visitor{};
            T::visitPostgresDefinition(visitor);
            return visitor.res_;
        }();
        return cache;
    }

    void accept(const char* const table, const char* const name) {
        if (!res_.empty()) {
            res_ += ",";
        }
        res_ += name;
        res_ += "=$";
        res_ += std::to_string(++index_);
    };

private:
    int index_ = 0;
    std::string res_;
};

template <typename T>
struct ExcludedAssigmentsList {
    static const std::string& get() {
        static const auto cache = [] {
            ExcludedAssigmentsList<T> visitor{};
            T::visitPostgresDefinition(visitor);
            return visitor.res_;
        }();
        return cache;
    }

    void accept(const char* const table, const char* const name) {
        if (!res_.empty()) {
            res_ += ",";
        }
        res_ += name;
        res_ += "=EXCLUDED.";
        res_ += name;
    };

private:
    std::string res_;
};

template <typename T>
struct PlaceholdersList {
    template <typename Iterator>
    static std::string generate(Iterator it, const Iterator end) {
        std::string res{};
        PlaceholdersList<T> visitor{};
        for (; it != end; ++it) {
            T::visitPostgresDefinition(visitor);
            res += res.empty() ? "(" : ",(";
            res += visitor.res_;
            res += ")";
            visitor.res_.clear();
        }
        return res;
    }

    static const std::string& get() {
        static const auto cache = [] {
            PlaceholdersList<T> visitor{};
            T::visitPostgresDefinition(visitor);
            return visitor.res_;
        }();
        return cache;
    }

    void accept(const char* const table, const char* const name) {
        res_ += res_.empty() ? "$" : ",$";
        res_ += std::to_string(++index_);
    };

private:
    int index_ = 0;
    std::string res_;
};

}  // namespace internal
}  // namespace postgres
