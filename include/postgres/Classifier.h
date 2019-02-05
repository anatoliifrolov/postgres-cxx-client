#pragma once

namespace postgres {
namespace internal {

template <int N>
struct Tag {
    char placeholder[N];
};

using ReadableTag = Tag<1>;
using VisitableTag = Tag<2>;
using OptionalTag = Tag<3>;

template <typename T>
constexpr ReadableTag classify(...) {
    return ReadableTag{};
}

template <typename T>
constexpr VisitableTag classify(decltype (T::k_has_serialiser)) {
    return VisitableTag{};
}

template <typename T>
constexpr OptionalTag classify(decltype (T{}.has_value())) {
    return OptionalTag{};
}

template <typename T, typename U>
constexpr bool isTagged() {
    return sizeof (classify<T>(true)) == sizeof (U);
}

template <typename T>
constexpr bool isReadable() {
    return isTagged<T, ReadableTag>();
}

template <typename T>
constexpr bool isVisitable() {
    return isTagged<T, VisitableTag>();
}

template <typename T>
constexpr bool isOptional() {
    return isTagged<T, OptionalTag>();
}

}  // namespace internal
}  // namespace postgres
