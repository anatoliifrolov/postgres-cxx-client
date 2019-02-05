#pragma once

#include <arpa/inet.h>
#include <utility>

namespace postgres {
namespace internal {

inline bool needReorder() {
    static const auto res = (htonl(1) != 1);
    return res;
}

template <typename T>
T orderBytes(T val) {
    static constexpr auto size = sizeof (T);
    if (needReorder()) {
        auto bytes = reinterpret_cast<char*>(&val);
        for (auto i = 0u; i < size / 2; ++i) {
            std::swap(bytes[i], bytes[size - 1 - i]);
        }
    }
    return val;
}

template <typename T>
T orderBytes(const char* const buf) {
    return orderBytes(*(reinterpret_cast<const T*>(buf)));
}

}  // namespace internal
}  // namespace postgres
