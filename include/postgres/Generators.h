#pragma once

#include <type_traits>
#include <postgres/internal/Visitors.h>

namespace postgres {

template <typename Visitable>
const std::string& listFields() {
    return internal::FieldsList<Visitable>::get();
}

template <typename Visitable>
const std::string& listPlaceholders() {
    return internal::PlaceholdersList<Visitable>::get();
}

template <typename Iterator>
std::string listPlaceholders(const Iterator it, const Iterator end) {
    using Visitable = std::remove_pointer_t<typename Iterator::value_type>;
    return internal::PlaceholdersList<Visitable>::generate(it, end);
}

template <typename Visitable>
const std::string& listAssigments() {
    return internal::AssigmentsList<Visitable>::get();
}

template <typename Visitable>
const std::string& listExcludedAssigments() {
    return internal::ExcludedAssigmentsList<Visitable>::get();
}

template <typename Visitable>
const std::string& insertStatement() {
    static auto const cache = "INSERT INTO "
                              + std::string{Visitable::_POSTGRES_CXX_TABLE_NAME}
                              + " ("
                              + listFields<Visitable>()
                              + ")"
                              + " VALUES ("
                              + listPlaceholders<Visitable>()
                              + ")";
    return cache;
}

template <typename Iterator>
std::string multiInsertStatement(const Iterator it, const Iterator end) {
    using Visitable = typename Iterator::value_type;
    return "INSERT INTO "
           + std::string{Visitable::_POSTGRES_CXX_TABLE_NAME}
           + " ("
           + listFields<Visitable>()
           + ")"
           + " VALUES "
           + listPlaceholders(it, end);
}

template <typename Visitable>
const std::string& insertStatementWeak() {
    static auto const cache = insertStatement<Visitable>() + " ON CONFLICT DO NOTHING";
    return cache;
}

template <typename Iterator>
std::string multiInsertStatementWeak(const Iterator it, const Iterator end) {
    return multiInsertStatement(it, end) + " ON CONFLICT DO NOTHING";
}

template <typename Visitable>
const std::string& updateStatement() {
    static auto const cache = "UPDATE "
                              + std::string{Visitable::_POSTGRES_CXX_TABLE_NAME}
                              + " SET "
                              + listAssigments<Visitable>();
    return cache;
}

template <typename Visitable>
const std::string& selectStatement() {
    static auto const cache = "SELECT "
                              + listFields<Visitable>()
                              + " FROM "
                              + Visitable::_POSTGRES_CXX_TABLE_NAME;
    return cache;
}

}  // namespace postgres
