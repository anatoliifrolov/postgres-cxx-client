#pragma once

#include <arpa/inet.h>
#include <utility>

namespace postgres {
namespace internal {

template <typename T>
T orderBytes(T val) {
    auto constexpr    LEN        = sizeof(T);
    static auto const IS_ORDERED = (LEN == 1) || (htonl(1) == 1);
    if (IS_ORDERED) {
        return val;
    }

    auto bytes = reinterpret_cast<char*>(&val);

    for (auto i = 0u; i < LEN / 2; ++i) {
        std::swap(bytes[i], bytes[LEN - 1 - i]);
    }
    return val;
}

template <typename T>
T orderBytes(const char* const buf) {
    return orderBytes(*(reinterpret_cast<const T*>(buf)));
}

}  // namespace internal
}  // namespace postgres
