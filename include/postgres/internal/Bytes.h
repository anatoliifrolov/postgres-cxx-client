#pragma once

#include <utility>

namespace postgres::internal {

template <typename T>
T orderBytes(T val) {
    union {
        uint16_t value;
        uint8_t  bytes[2];
    } constexpr TEST = {0x0102};

    auto constexpr    LEN   = sizeof(T);
    static auto const is_ok = (LEN == 1) || (TEST.bytes[0] == 0x01);
    if (is_ok) {
        return val;
    }

    auto bytes = reinterpret_cast<uint8_t*>(&val);

    for (auto i = 0u; i < LEN / 2; ++i) {
        std::swap(bytes[i], bytes[LEN - 1 - i]);
    }
    return val;
}

template <typename T>
T orderBytes(const char* const buf) {
    return orderBytes(*(reinterpret_cast<const T*>(buf)));
}

}  // namespace postgres::internal
