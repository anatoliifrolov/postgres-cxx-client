#pragma once

#include <string>
#include <type_traits>
#include <postgres/internal/Visitors.h>

namespace postgres {

template <typename T>
struct Statement {
    static std::string const& create() {
        static auto const cache = "CREATE TABLE "
                                  + std::string{table()}
                                  + " ("
                                  + typedFields()
                                  + ")";
        return cache;
    }

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
        static auto const cache = collect<internal::FieldsCollector>();
        return cache;
    }

    static std::string const& typedFields() {
        static auto const cache = collect<internal::TypedFieldsCollector>();
        return cache;
    }

    static std::string const& placeholders() {
        static auto const cache = collect<internal::PlaceholdersCollector>();
        return cache;
    }

    static std::string const& assignments() {
        static auto const cache = collect<internal::AssignmentsCollector>();
        return cache;
    }

    static char const* table() {
        return T::_POSTGRES_CXX_TABLE_NAME;
    }

private:
    template <typename C>
    static std::string collect() {
        C coll{};
        T::visitPostgresDefinition(coll);
        return coll.res;
    }
};

struct RangeStatement {
    template <typename Iter>
    static std::string insert(Iter const beg, Iter const end) {
        using T = std::remove_pointer_t<typename Iter::value_type>;
        using S = Statement<T>;
        return "INSERT INTO "
               + std::string{S::table()}
               + " ("
               + S::fields()
               + ") VALUES "
               + placeholders(beg, end);
    }

    template <typename Iter>
    static std::string placeholders(Iter const beg, Iter const end) {
        using T = std::remove_pointer_t<typename Iter::value_type>;
        internal::PlaceholdersCollector coll{};
        std::string                     res{};

        for (auto it = beg; it != end; ++it) {
            T::visitPostgresDefinition(coll);
            res += res.empty() ? "(" : ",(";
            res += coll.res;
            res += ")";
            coll.res.clear();
        }
        return res;
    }
};

}  // namespace postgres
