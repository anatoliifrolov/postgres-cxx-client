#pragma once

#include <type_traits>
#include <postgres/Visitors.h>

namespace postgres {

template <typename Visitable>
const std::string& listFields() {
    return internal::VisitableFields<Visitable>::get();
}

template <typename Visitable>
const std::string& listPlaceholders() {
    return internal::VisitablePlaceholders<Visitable>::get();
}

template <typename Iterator>
std::string listPlaceholders(const Iterator it, const Iterator end) {
    using Visitable = std::remove_pointer_t<typename Iterator::value_type>;
    return internal::VisitablePlaceholders<Visitable>::generate(it, end);
}

template <typename Visitable>
const std::string& listAssigments() {
    return internal::VisitableAssigments<Visitable>::get();
}

template <typename Visitable>
const std::string& listExcludedAssigments() {
    return internal::VisitableExcludedAssigments<Visitable>::get();
}

template <typename Visitable>
const std::string& insertStatement() {
    static const auto cache =
        "INSERT INTO " + std::string{Visitable::_POSTGRES_CXX_TABLE_NAME} +
        " (" + listFields<Visitable>() + ")" +
        " VALUES (" + listPlaceholders<Visitable>() + ")";
    return cache;
}

template <typename Iterator>
std::string multiInsertStatement(const Iterator it, const Iterator end) {
    using Visitable = typename Iterator::value_type;
    return "INSERT INTO " + std::string{Visitable::_POSTGRES_CXX_TABLE_NAME} +
        " (" + listFields<Visitable>() + ")" +
        " VALUES " + listPlaceholders(it, end);
}

template <typename Visitable>
const std::string& insertStatementWeak() {
    static const auto cache = insertStatement<Visitable>() + " ON CONFLICT DO NOTHING";
    return cache;
}

template <typename Iterator>
std::string multiInsertStatementWeak(const Iterator it, const Iterator end) {
    return multiInsertStatement(it, end) + " ON CONFLICT DO NOTHING";
}

template <typename Visitable>
const std::string& updateStatement() {
    static const auto cache =
        "UPDATE " + std::string{Visitable::_POSTGRES_CXX_TABLE_NAME} +
        " SET " + listAssigments<Visitable>();
    return cache;
}

template <typename Visitable>
const std::string& selectStatement() {
    static const auto cache =
        "SELECT " + listFields<Visitable>() +
        " FROM " + Visitable::_POSTGRES_CXX_TABLE_NAME;
    return cache;
}

}  // namespace postgres
