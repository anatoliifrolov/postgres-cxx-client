#pragma once

#include <type_traits>
#include <postgres/internal/Visitors.h>

namespace postgres {

template <typename T>
class Statement {
public:
    explicit Statement() = delete;
    Statement(Statement const& other) = delete;
    Statement& operator=(Statement const& other) = delete;
    Statement(Statement&& other) = delete;
    Statement& operator=(Statement&& other) = delete;
    ~Statement() = delete;

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

    static std::string const& insertWeak() {
        static auto const cache = insert() + " ON CONFLICT DO NOTHING";
        return cache;
    }

    template <typename Iter>
    static std::string multiInsert(Iter const it, Iter const end) {
        return "INSERT INTO "
               + std::string{table()}
               + " ("
               + fields()
               + ") VALUES "
               + multiPlaceholders(it, end);
    }

    template <typename Iter>
    static std::string multiInsertWeak(Iter const it, Iter const end) {
        return multiInsert(it, end) + " ON CONFLICT DO NOTHING";
    }

    static std::string const& update() {
        static auto const cache = "UPDATE " + std::string{table()} + " SET " + assigments();
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

    template <typename Iter>
    static std::string multiPlaceholders(Iter it, Iter const end) {
        static_assert(std::is_same_v<T, std::remove_pointer_t<typename Iter::value_type>>,
                      "Iterator has mismatched value type");
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

    static std::string const& assigments() {
        static auto const cache = [] {
            internal::AssigmentsCollector<T> coll{};
            T::visitPostgresDefinition(coll);
            return coll.res_;
        }();
        return cache;
    }

    static std::string const& excludedAssigments() {
        static auto const cache = [] {
            internal::ExcludedAssigmentsCollector<T> coll{};
            T::visitPostgresDefinition(coll);
            return coll.res_;
        }();
        return cache;
    }

    static char const* table() {
        return T::_POSTGRES_CXX_TABLE_NAME;
    }
};

}  // namespace postgres
