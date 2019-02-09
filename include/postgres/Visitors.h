#pragma once

#include <string>
#include <postgres/internal/Classifier.h>

namespace postgres {
namespace internal {

template <typename T>
struct VisitableFields {
    static const std::string& get() {
        static const auto cache = [] {
            VisitableFields<T> visitor{};
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
struct VisitableAssigments {
    static const std::string& get() {
        static const auto cache = [] {
            VisitableAssigments<T> visitor{};
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
struct VisitableExcludedAssigments {
    static const std::string& get() {
        static const auto cache = [] {
            VisitableExcludedAssigments<T> visitor{};
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
struct VisitablePlaceholders {
    template <typename Iterator>
    static std::string generate(Iterator it, const Iterator end) {
        std::string res{};
        VisitablePlaceholders<T> visitor{};
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
            VisitablePlaceholders<T> visitor{};
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
