#pragma once

#include <type_traits>
#include <postgres/internal/Visitors.h>

namespace postgres {

template <typename T>
struct Statement {
    static std::string const& insert() {
        static auto const cache = "INSERT INTO "
                                  + std::string{table()}
                                  + " ("
                                  + fields()
                                  + ") VALUES ("
                                  + placeholders()
                                  + ")";
        return cache;
    }

    static std::string const& update() {
        static auto const cache = "UPDATE " + std::string{table()} + " SET " + assignments();
        return cache;
    }

    static std::string const& select() {
        static auto const cache = "SELECT " + fields() + " FROM " + table();
        return cache;
    }

    static std::string const& fields() {
        static auto const cache = [] {
            internal::FieldsCollector<T> coll{};
            T::visitPostgresDefinition(coll);
            return coll.res_;
        }();
        return cache;
    }

    static std::string const& placeholders() {
        static auto const cache = [] {
            internal::PlaceholdersCollector<T> coll{};
            T::visitPostgresDefinition(coll);
            return coll.res_;
        }();
        return cache;
    }

    static std::string const& assignments() {
        static auto const cache = [] {
            internal::AssignmentsCollector<T> coll{};
            T::visitPostgresDefinition(coll);
            return coll.res_;
        }();
        return cache;
    }

    static char const* table() {
        return T::_POSTGRES_CXX_TABLE_NAME;
    }
};

struct RangeStatement {
    template <typename Iter>
    static std::string insert(Iter const it, Iter const end) {
        using T = std::remove_pointer_t<typename Iter::value_type>;
        using S = Statement<T>;
        return "INSERT INTO "
               + std::string{S::table()}
               + " ("
               + S::fields()
               + ") VALUES "
               + placeholders(it, end);
    }

    template <typename Iter>
    static std::string placeholders(Iter it, Iter const end) {
        using T = std::remove_pointer_t<typename Iter::value_type>;
        std::string                        res{};
        internal::PlaceholdersCollector<T> coll{};
        for (; it != end; ++it) {
            T::visitPostgresDefinition(coll);
            res += res.empty() ? "(" : ",(";
            res += coll.res_;
            res += ")";
            coll.res_.clear();
        }
        return res;
    }
};

}  // namespace postgres
